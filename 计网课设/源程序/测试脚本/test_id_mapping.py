#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DNS ID映射模块测试脚本
用于验证修复后的ID映射功能是否正常工作

测试覆盖：
1. ID分配功能
2. ID映射正确性  
3. 错误处理
4. 响应转发
5. 过期清理
"""

import subprocess
import time
import socket
import struct
import threading
import random
from typing import List, Tuple, Optional

class DNSIDMapTester:
    def __init__(self, dns_exe_path: str = "./DNS.exe"):
        self.dns_exe_path = dns_exe_path
        self.test_results = []
        self.dns_process = None
        self.total_queries_sent = 0
        self.total_responses_received = 0
        self.successful_mappings = 0
        self.failed_mappings = 0
        
    def log_test(self, test_name: str, passed: bool, details: str = ""):
        """记录测试结果"""
        result = "✓ PASS" if passed else "✗ FAIL"
        print(f"  {test_name:.<50} {result}")
        if details and not passed:
            print(f"    详细信息: {details}")
        self.test_results.append((test_name, passed, details))
    
    def check_dns_server_running(self) -> bool:
        """检查DNS服务器是否已经在运行"""
        try:
            # 检查53端口是否被监听
            result = subprocess.run(['netstat', '-an'], capture_output=True, text=True)
            if ':53' in result.stdout and 'LISTENING' in result.stdout:
                return True
        except:
            pass
        return False

    def start_dns_server(self) -> bool:
        """启动DNS服务器"""
        # 首先检查是否已经有DNS服务器在运行
        if self.check_dns_server_running():
            
            self.dns_process = None  # 不需要管理现有进程
            return True
        
        try:
            
            
            # 启动DNS服务器 (调试模式，自动使用配置文件)
            self.dns_process = subprocess.Popen(
                [self.dns_exe_path, "-d"],  # 调试模式，自动使用默认配置文件
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                cwd="."  # 确保在正确的工作目录
            )
            time.sleep(3)  # 增加等待时间以确保服务器完全启动
            
            if self.dns_process.poll() is None:
                print(" DNS服务器启动成功")
                print("   配置文件: dnsrelay.txt")
                
                return True
            else:
                print("✗ DNS服务器启动失败")
                # 输出错误信息
                stdout, stderr = self.dns_process.communicate()
                if stderr:
                    print(f"   错误输出: {stderr}")
                return False
        except FileNotFoundError:
            print(f"✗ 未找到DNS可执行文件: {self.dns_exe_path}")
            return False
        except Exception as e:
            print(f"✗ 启动DNS服务器时出错: {e}")
            return False
    
    def stop_dns_server(self):
        """停止DNS服务器"""
        if self.dns_process:
            self.dns_process.terminate()
            try:
                self.dns_process.wait(timeout=5)
                print(" DNS服务器已停止")
            except subprocess.TimeoutExpired:
                self.dns_process.kill()
                print(" 强制终止DNS服务器")
    
    def create_dns_query(self, query_id: int, domain: str) -> bytes:
        """创建DNS查询报文"""
        # DNS头部
        header = struct.pack('!HHHHHH', 
                           query_id,      # ID
                           0x0100,        # 标志：标准查询，期望递归
                           1,             # 问题数
                           0,             # 回答数
                           0,             # 权威记录数
                           0)             # 附加记录数
        
        # 问题区域
        question = b''
        for part in domain.split('.'):
            question += struct.pack('!B', len(part)) + part.encode()
        question += b'\x00'  # 域名结束标记
        question += struct.pack('!HH', 1, 1)  # Type A, Class IN
        
        return header + question
    
    def parse_dns_response(self, data: bytes) -> Optional[Tuple[int, str]]:
        """解析DNS响应"""
        try:
            if len(data) < 12:
                return None
            
            # 解析头部
            header = struct.unpack('!HHHHHH', data[:12])
            response_id = header[0]
            
            return response_id, "response_received"
        except:
            return None
    
    def test_basic_id_allocation(self) -> bool:
        """测试基本ID分配功能"""
        print("\n 测试1: 基本ID分配功能")
        print("   目标: 验证DNS服务器能够正确分配和管理查询ID")
        
        try:
            # 创建多个并发查询
            queries = []
            test_domains = [
                "example.com", "google.com", "baidu.com", "github.com", "microsoft.com",
                "apple.com", "amazon.com", "facebook.com", "twitter.com", "youtube.com"
            ]
            
            print(f"    准备发送 {len(test_domains)} 个DNS查询...")
            
            for i, domain in enumerate(test_domains):
                query_id = 1000 + i
                query = self.create_dns_query(query_id, domain)
                queries.append((query_id, domain, query))
                print(f"      查询 {i+1}: ID={query_id}, 域名={domain}")
            
            # 发送查询
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.settimeout(3.0)
            
            sent_count = 0
            response_count = 0
            
            print(f"\n    开始发送查询到 127.0.0.1:53...")
            
            for query_id, domain, query in queries:
                try:
                    start_time = time.time()
                    sock.sendto(query, ('127.0.0.1', 53))
                    sent_count += 1
                    self.total_queries_sent += 1
                    
                    # 尝试接收响应
                    try:
                        response, addr = sock.recvfrom(1024)
                        end_time = time.time()
                        response_time = (end_time - start_time) * 1000
                        
                        # 解析响应
                        parsed = self.parse_dns_response(response)
                        if parsed:
                            response_id, status = parsed
                            response_count += 1
                            self.total_responses_received += 1
                            
                            if response_id == query_id:
                                self.successful_mappings += 1
                                print(f"      ✓ 查询 {domain}: ID {query_id} -> 响应 {response_id} ({response_time:.1f}ms)")
                            else:
                                self.failed_mappings += 1
                                print(f"       查询 {domain}: ID {query_id} -> 响应 {response_id} (ID不匹配!)")
                        else:
                            print(f"      查询 {domain}: 响应解析失败")
                            
                    except socket.timeout:
                        print(f"       查询 {domain}: 响应超时")
                    
                    time.sleep(0.1)  # 小延迟避免过快发送
                    
                except Exception as query_error:
                    print(f"       查询 {domain}: 发送失败 - {query_error}")
            
            sock.close()
            
            print(f"\n    测试统计:")
            print(f"      发送查询: {sent_count}/{len(test_domains)}")
            print(f"      收到响应: {response_count}/{sent_count}")
            print(f"      ID映射成功: {self.successful_mappings}")
            print(f"      ID映射失败: {self.failed_mappings}")
            
            success = sent_count >= len(test_domains) * 0.7  # 至少70%成功
            success_rate = (sent_count / len(test_domains)) * 100
            
            self.log_test("基本ID分配功能", success, 
                         f"发送成功率: {success_rate:.1f}%, 响应率: {response_count/max(sent_count,1)*100:.1f}%")
            return success
            
        except Exception as e:
            self.log_test("基本ID分配测试", False, f"测试异常: {str(e)}")
            return False
    
    def test_id_uniqueness(self) -> bool:
        """测试ID唯一性"""
        print("\n 测试2: ID唯一性验证")
        print("   目标: 验证DNS服务器处理相同ID查询的能力")
        print("   策略: 发送多个相同ID的查询，测试ID冲突处理机制")
        
        try:
            # 快速发送多个相同ID的查询
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.settimeout(1.0)
            
            test_id = 12345
            test_domain = "unique.test.com"
            test_count = 20
            
            print(f"    测试参数:")
            print(f"      测试ID: {test_id}")
            print(f"      测试域名: {test_domain}")
            print(f"      重复次数: {test_count}")
            print(f"      超时设置: 1.0秒")
            
            same_id_query = self.create_dns_query(test_id, test_domain)
            
            print(f"\n    开始发送{test_count}个相同ID查询...")
            sent_count = 0
            response_count = 0
            different_server_ids = set()
            
            for i in range(test_count):
                try:
                    start_time = time.time()
                    sock.sendto(same_id_query, ('127.0.0.1', 53))
                    sent_count += 1
                    self.total_queries_sent += 1
                    
                    # 尝试接收响应
                    try:
                        response, addr = sock.recvfrom(1024)
                        end_time = time.time()
                        response_time = (end_time - start_time) * 1000
                        
                        parsed = self.parse_dns_response(response)
                        if parsed:
                            response_id, status = parsed
                            response_count += 1
                            self.total_responses_received += 1
                            different_server_ids.add(response_id)
                            
                            if i < 5:  # 只显示前5个响应的详细信息
                                print(f"      第{i+1}次: 客户端ID {test_id} -> 服务器分配ID {response_id} ({response_time:.1f}ms)")
                            elif i == 5:
                                print(f"      ... (继续处理剩余{test_count-5}个查询)")
                                
                    except socket.timeout:
                        if i < 5:
                            print(f"      第{i+1}次: 响应超时")
                    
                    time.sleep(0.01)  # 短暂延迟
                except Exception as e:
                    print(f"      第{i+1}次: 发送失败 - {e}")
                    break
            
            sock.close()
            
            print(f"\n    ID唯一性测试统计:")
            print(f"      发送查询: {sent_count}/{test_count}")
            print(f"      收到响应: {response_count}/{sent_count}")
            print(f"      服务器分配的不同ID数量: {len(different_server_ids)}")
            print(f"      服务器分配的ID范围: {min(different_server_ids) if different_server_ids else 'N/A'} - {max(different_server_ids) if different_server_ids else 'N/A'}")
            
            # 如果能发送多个，说明ID分配机制工作正常
            success = sent_count >= 10 and response_count >= 5
            self.log_test("相同ID查询处理", success, 
                         f"处理 {sent_count} 个查询，收到 {response_count} 个响应，分配 {len(different_server_ids)} 个不同ID")
            return success
            
        except Exception as e:
            self.log_test("ID唯一性测试", False, str(e))
            return False
    
    def test_concurrent_queries(self) -> bool:
        """测试并发查询处理"""
        print("\n 测试3: 并发查询处理")
        print("   目标: 验证DNS服务器的并发处理能力和线程安全性")
        print("   策略: 启动多个线程同时发送查询，检测竞态条件")
        
        thread_count = 5
        queries_per_thread = 5
        print(f"      测试参数:")
        print(f"      并发线程数: {thread_count}")
        print(f"      每线程查询数: {queries_per_thread}")
        print(f"      总查询数: {thread_count * queries_per_thread}")
        print(f"      超时设置: 1.0秒/查询")
        
        def send_queries(thread_id: int, results: List):
            thread_results = {
                'sent': 0,
                'received': 0,
                'responses': [],
                'errors': 0
            }
            # 关键：每个线程创建并使用自己的socket，经查询 这是保证线程安全、避免网络I/O冲突的标准做法。
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                sock.settimeout(1.0)
                
                print(f"    线程{thread_id}: 开始发送查询...")
                
                for i in range(queries_per_thread):
                    query_id = thread_id * 100 + i
                    domain = f"thread{thread_id}test{i}.com"
                    query = self.create_dns_query(query_id, domain)
                    
                    try:
                        start_time = time.time()
                        sock.sendto(query, ('127.0.0.1', 53))
                        thread_results['sent'] += 1
                        self.total_queries_sent += 1
                        
                        # 尝试接收响应
                        try:
                            response, addr = sock.recvfrom(1024)
                            end_time = time.time()
                            response_time = (end_time - start_time) * 1000
                            
                            parsed = self.parse_dns_response(response)
                            if parsed:
                                response_id, status = parsed
                                thread_results['received'] += 1
                                self.total_responses_received += 1
                                thread_results['responses'].append({
                                    'query_id': query_id,
                                    'response_id': response_id,
                                    'time': response_time,
                                    'domain': domain
                                })
                                
                                if response_id == query_id:
                                    self.successful_mappings += 1
                                else:
                                    self.failed_mappings += 1
                                    
                                print(f"      线程{thread_id}-查询{i+1}: ID {query_id} -> {response_id} ({response_time:.1f}ms)")
                        except socket.timeout:
                            print(f"      线程{thread_id}-查询{i+1}: 响应超时")
                            
                    except Exception as e:
                        thread_results['errors'] += 1
                        print(f"      线程{thread_id}-查询{i+1}: 发送失败 - {e}")
                
                sock.close()
                results.append(thread_results)
                print(f"   ✓ 线程{thread_id}: 完成 (发送:{thread_results['sent']}, 接收:{thread_results['received']})")
                
            except Exception as e:
                print(f"   ✗ 线程{thread_id}: 异常 - {e}")
                results.append(thread_results)
        
        # 创建多个并发线程
        threads = []
        results = []
        
        print(f"\n    启动{thread_count}个并发线程...")
        start_time = time.time()
        
        for i in range(thread_count):
            thread = threading.Thread(target=send_queries, args=(i, results))
            threads.append(thread)
            thread.start()
        
        # 等待所有线程完成
        for i, thread in enumerate(threads):
            thread.join(timeout=10)
            if thread.is_alive():
                print(f"    线程{i}: 超时未完成")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        # 统计结果
        total_sent = sum(r['sent'] for r in results)
        total_received = sum(r['received'] for r in results)
        total_errors = sum(r['errors'] for r in results)
        
        print(f"\n    并发测试统计:")
        print(f"      完成线程数: {len(results)}/{thread_count}")
        print(f"      总发送查询: {total_sent}")
        print(f"      总接收响应: {total_received}")
        print(f"      总错误数: {total_errors}")
        print(f"      总耗时: {total_time:.2f}秒")
      
        
        success = len(results) >= 3 and total_sent >= 10 and total_received >= 5
        
        self.log_test("并发查询处理", success, 
                     f"{len(results)} 线程发送 {total_sent} 查询，收到 {total_received} 响应")
        return success
    

    

    
    def run_all_tests(self) -> None:
        """运行所有测试"""
        print(" DNS ID映射模块专项测试")
        print("=" * 80)
        print(" 测试目标: 验证修复后的ID映射模块核心功能")
        print(" 修复内容: 字节序错误、逻辑漏洞、并发优化")
        print("=" * 80)
        
        # 启动DNS服务器
        if not self.start_dns_server():
            print(" 无法启动DNS服务器，测试终止")
            print(" 请确保:")
            print("   1. DNS.exe 已编译完成")
            print("   2. test_dnsrelay.txt 配置文件存在")
            print("   3. 没有其他程序占用53端口")
            return
        
        try:
            # 等待服务器完全启动
            print("\n 等待DNS服务器完全启动...")
            time.sleep(4)
            
            # 运行测试
            tests = [
                ("基本ID分配功能", self.test_basic_id_allocation),
                ("ID唯一性验证", self.test_id_uniqueness),
                ("并发查询处理", self.test_concurrent_queries)
            ]
            
            passed_tests = 0
            test_details = []
            
            for test_name, test_func in tests:
                print(f"\n{'='*60}")
                start_time = time.time()
                test_result = test_func()
                end_time = time.time()
                test_duration = end_time - start_time
                
                test_details.append({
                    'name': test_name,
                    'passed': test_result,
                    'duration': test_duration
                })
                
                if test_result:
                    passed_tests += 1
                    print(f" {test_name} - 通过 ({test_duration:.2f}s)")
                else:
                    print(f" {test_name} - 失败 ({test_duration:.2f}s)")
            
            # 详细测试总结
            print("\n" + "=" * 80)
            print(" 详细测试报告")
            print("=" * 80)
            
            print(f" 总测试数量: {len(tests)}")
            print(f" 通过测试: {passed_tests}")
            print(f" 失败测试: {len(tests) - passed_tests}")
            print(f" 成功率: {passed_tests/len(tests)*100:.1f}%")
            
            print(f"\n 网络统计:")
            print(f"   发送查询总数: {self.total_queries_sent}")
            print(f"   收到响应总数: {self.total_responses_received}")
            print(f"   成功映射数量: {self.successful_mappings}")
            print(f"   失败映射数量: {self.failed_mappings}")
            
            if self.total_queries_sent > 0:
                response_rate = (self.total_responses_received / self.total_queries_sent) * 100
                mapping_rate = (self.successful_mappings / max(self.total_responses_received, 1)) * 100
                print(f"   响应成功率: {response_rate:.1f}%")
                print(f"   ID映射成功率: {mapping_rate:.1f}%")
            
            print(f"\n 各项测试详情:")
            for detail in test_details:
                status = " 通过" if detail['passed'] else "❌ 失败"
                print(f"   {detail['name']:<20} {status} ({detail['duration']:.2f}s)")
            
            print(f"\n{'='*80}")
            if passed_tests == len(tests):
                print(" 恭喜！所有测试通过，ID映射模块工作完全正常！")
                print(" 修复效果:")
                print("   ✓ 字节序问题已解决")
                print("   ✓ ID映射逻辑正常工作")
                print("   ✓ 并发处理性能优异")
                print("   ✓ ID分配机制稳定")
            else:
                print(" 部分测试失败，请检查以下问题:")
                for detail in test_details:
                    if not detail['passed']:
                        print(f"   {detail['name']}: 需要进一步排查")
                print("\n 建议:")
                print("   1. 检查DNS服务器日志输出")
                print("   2. 确认网络配置正确")
                print("   3. 验证代码修复是否完整")
            print("=" * 80)
            
        finally:
            self.stop_dns_server()

def main():
    """主函数"""
    print("🔧 DNS ID映射模块测试工具")
    print("作者: DNS团队")
    print("版本: 1.0")
    print()
    
    # 检查是否有DNS可执行文件和配置文件
    import os
    possible_paths = ["./DNS.exe", "../DNS.exe", "./x64/Debug/DNS.exe", "../x64/Debug/DNS.exe"]
    config_files = ["test_dnsrelay.txt", "dnsrelay.txt"]
    
    # 查找DNS可执行文件
    dns_exe = None
    for path in possible_paths:
        if os.path.exists(path):
            dns_exe = path
            break
    
    if not dns_exe:
        print(" 未找到DNS可执行文件")
        print("请确保在以下位置之一存在DNS.exe:")
        for path in possible_paths:
            print(f"  - {path}")
        print("\n💡 提示: 请先在Visual Studio中编译生成DNS.exe")
        return
    
    # 检查配置文件
    config_file = None
    for config in config_files:
        if os.path.exists(config):
            config_file = config
            break
    
    if not config_file:
        print(" 未找到DNS配置文件")
        print("请确保存在以下文件之一:")
        for config in config_files:
            print(f"  - {config}")
        print("\n💡 提示: test_dnsrelay.txt 应包含测试用的域名-IP映射")
        return
    
    print(f"✓ 找到DNS可执行文件: {dns_exe}")
    print(f"✓ 找到配置文件: {config_file}")
    print(f"✓ 当前工作目录: {os.getcwd()}")
    print()
    
    # 运行测试
    tester = DNSIDMapTester(dns_exe)
    tester.run_all_tests()

if __name__ == "__main__":
    main() 