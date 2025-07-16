#!/usr/bin/env python3
"""
O(1) LRU Cache 专门测试脚本
压力测试
使用自定义hosts文件中的域名测试缓存性能
"""

import socket
import struct
import time
import random

def create_dns_query(domain, query_id=1234):
    """创建DNS查询包"""
    # DNS Header (12 bytes)
    flags = 0x0100  # 标准查询，期望递归
    questions = 1
    answer_rrs = 0
    authority_rrs = 0
    additional_rrs = 0
    
    header = struct.pack('!HHHHHH', query_id, flags, questions, 
                        answer_rrs, authority_rrs, additional_rrs)
    
    # DNS Question
    question = b''
    for part in domain.split('.'):
        question += struct.pack('B', len(part)) + part.encode()
    question += b'\x00'  # 结束标记
    question += struct.pack('!HH', 1, 1)  # Type A, Class IN
    
    return header + question

def send_dns_query(domain, server_ip='127.0.0.1', port=53):
    """发送DNS查询并测量响应时间"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(2)
    
    try:
        query_id = random.randint(1, 65535)
        query = create_dns_query(domain, query_id)
        
        start_time = time.time()
        sock.sendto(query, (server_ip, port))
        response, addr = sock.recvfrom(512)
        end_time = time.time()
        
        response_time = (end_time - start_time) * 1000  # 转换为毫秒
        
        # 解析响应获取IP地址
        if len(response) >= 12:
            # 跳过header和question，找到answer
            answer_start = 12
            # 跳过question域名
            pos = answer_start
            while pos < len(response) and response[pos] != 0:
                label_len = response[pos]
                if label_len > 63:  # 压缩标签
                    pos += 2
                    break
                pos += label_len + 1
            pos += 1 + 4  # 跳过结束符和type+class
            
            # 现在应该在answer section
            if pos + 12 <= len(response):
                # 跳过name (2字节压缩指针), type (2字节), class (2字节), ttl (4字节), rdlength (2字节)
                pos += 12
                if pos + 4 <= len(response):
                    ip = '.'.join(str(b) for b in response[pos:pos+4])
                    return response_time, True, ip
        
        return response_time, True, "Unknown"
        
    except socket.timeout:
        return None, False, None
    except Exception as e:
        print(f"查询 {domain} 时出错: {e}")
        return None, False, None
    finally:
        sock.close()

def test_o1_lru_cache():
    """测试O(1) LRU Cache性能"""
    print(" O(1) LRU Cache 专项性能测试")
    print("=" * 60)
    
    # 我们hosts文件中的测试域名 - 扩展到100+条
    test_domains = [
        # 原有测试域名
        'test1',
        'cache.test', 
        'performance.benchmark',
        'localhost.test',
        'dns.google.test',
        'sina.com.cn',
        'sohu.com',
        'bupt.edu.cn',
        'baidu.com',
        'dns114.test',
        'www.test1.com',
        'www.test2.com',
        'www.test3.com',
        'CaSe.TeSt.CoM',
        'case.test.com',
        'blocked.ad.com',
        
        # 新增大量测试域名
        'google.com',
        'facebook.com',
        'twitter.com',
        'youtube.com',
        'linkedin.com',
        'instagram.com',
        'microsoft.com',
        'apple.com',
        'amazon.com',
        'netflix.com',
        'spotify.com',
        'github.com',
        'stackoverflow.com',
        'reddit.com',
        'wikipedia.org',
        'mozilla.org',
        'ubuntu.com',
        'debian.org',
        'kernel.org',
        'gnu.org',
        'python.org',
        'nodejs.org',
        'reactjs.org',
        'angular.io',
        'vuejs.org',
        'bootstrap.com',
        'jquery.com',
        'webpack.js.org',
        'npmjs.com',
        'yarnpkg.com',
        'docker.com',
        'kubernetes.io',
        'jenkins.io',
        'gitlab.com',
        'bitbucket.org',
        'atlassian.com',
        'slack.com',
        'discord.com',
        'zoom.us',
        'teams.microsoft.com',
        'dropbox.com',
        'onedrive.live.com',
        'drive.google.com',
        'icloud.com',
        'adobe.com',
        'salesforce.com',
        'oracle.com',
        'ibm.com',
        'intel.com',
        'nvidia.com',
        'amd.com',
        'qualcomm.com',
        'samsung.com',
        'lg.com',
        'sony.com',
        'panasonic.com',
        'toshiba.com',
        'hitachi.com',
        'mitsubishi.com',
        'fujitsu.com',
        'nec.com',
        'canon.com',
        'nikon.com',
        'olympus.com',
        'leica.com',
        'zeiss.com',
        'tesla.com',
        'bmw.com',
        'mercedes-benz.com',
        'audi.com',
        'volkswagen.com',
        'ford.com',
        'gm.com',
        'toyota.com',
        'honda.com',
        'nissan.com',
        'mazda.com',
        'subaru.com',
        'hyundai.com',
        'kia.com',
        'boeing.com',
        'airbus.com',
        'nasa.gov',
        'spacex.com',
        'tesla.com',
        'paypal.com',
        'visa.com',
        'mastercard.com',
        'americanexpress.com',
        'chase.com',
        'bankofamerica.com',
        'wellsfargo.com',
        'citibank.com',
        'hsbc.com',
        'jpmorgan.com',
        'goldmansachs.com',
        'morganstanley.com',
        'blackrock.com',
        'vanguard.com',
        'fidelity.com',
        'schwab.com',
        'etrade.com',
        'robinhood.com',
        'coinbase.com',
        'binance.com',
        'kraken.com',
        'gemini.com',
        'blockchain.com',
        'cnn.com',
        'bbc.com',
        'reuters.com',
        'bloomberg.com',
        'wsj.com',
        'nytimes.com',
        'theguardian.com',
        'washingtonpost.com',
        'forbes.com',
        'techcrunch.com',
        'wired.com',
        'ars-technica.com',
        'engadget.com',
        'verge.com',
        'mashable.com',
        'buzzfeed.com'
    ]
    
    print(f" 测试域名数量: {len(test_domains)}")
    print(f" 每轮测试包含 {len(test_domains)} 个域名查询 (超过100条域名)")
    print(f" 用于充分测试O(1) LRU Cache的性能和稳定性")
    print()
    
    # 第一轮查询 - 缓存为空，所有查询都会miss
    print(" 第一轮查询 (缓存初始填充):")
    print("-" * 50)
    first_round_times = []
    first_round_results = []
    
    for i, domain in enumerate(test_domains):
        response_time, success, ip = send_dns_query(domain)
        if response_time:
            first_round_times.append(response_time)
            first_round_results.append((domain, response_time, ip))
            print(f"  {i+1:2d}. {domain:25s} -> {response_time:6.2f}ms  IP: {ip}")
        else:
            print(f"  {i+1:2d}. {domain:25s} -> 查询失败")
        time.sleep(0.05)  # 小延迟避免过快查询
    
    if first_round_times:
        avg_first = sum(first_round_times) / len(first_round_times)
        print(f"\n 第一轮平均响应时间: {avg_first:.2f}ms")
    
    print("\n 等待1秒后开始第二轮测试...")
    time.sleep(1)
    
    # 第二轮查询 - 相同域名，应该全部从缓存命中
    print("\n第二轮查询 (测试缓存命中):")
    print("-" * 50)
    second_round_times = []
    
    for i, domain in enumerate(test_domains):
        response_time, success, ip = send_dns_query(domain)
        if response_time:
            second_round_times.append(response_time)
            print(f"  {i+1:2d}. {domain:25s} -> {response_time:6.2f}ms  IP: {ip}")
        else:
            print(f"  {i+1:2d}. {domain:25s} -> 查询失败")
        time.sleep(0.05)
    
    if second_round_times:
        avg_second = sum(second_round_times) / len(second_round_times)
        print(f"\n 第二轮平均响应时间: {avg_second:.2f}ms")
    
    # 性能对比分析
    if first_round_times and second_round_times:
        improvement = ((avg_first - avg_second) / avg_first) * 100
        speedup = avg_first / avg_second
        
        print(f"\n 性能提升分析:")
        print(f"   响应时间改善: {improvement:.1f}%")
        print(f"   速度提升倍数: {speedup:.1f}x")
        
        if improvement > 30:
            print(f"    优秀！O(1) LRU Cache 显著提升了性能")
        elif improvement > 10:
            print(f"   良好！缓存机制工作正常")
        else:
            print(f"     提升有限，可能需要检查缓存实现")
    
    # 第三轮：随机访问测试LRU策略
    print(f"\n 第三轮查询 (随机访问测试LRU策略):")
    print("-" * 50)
    print("   随机查询50次，观察缓存命中情况...")
    
    third_round_times = []
    for i in range(50):
        domain = random.choice(test_domains)
        response_time, success, ip = send_dns_query(domain)
        if response_time:
            third_round_times.append(response_time)
            print(f"   {i+1:2d}. {domain:25s} -> {response_time:6.2f}ms")
        time.sleep(0.03)
    
    if third_round_times:
        avg_third = sum(third_round_times) / len(third_round_times)
        print(f"\n 随机访问平均响应时间: {avg_third:.2f}ms")
    
    # 最终总结
    print(f"\n 测试总结:")
    print(f"   第一轮(缓存miss): {avg_first:.2f}ms")
    print(f"   第二轮(缓存hit):  {avg_second:.2f}ms") 
    print(f"   第三轮(随机访问): {avg_third:.2f}ms")
    print(f"   性能提升: {improvement:.1f}% ({speedup:.1f}x)")

def test_hash_collision():
    """测试哈希冲突情况"""
    print(f"\n 哈希冲突测试:")
    print("-" * 30)
    
    # 测试相似域名的哈希分布
    similar_domains = [
        'www.test1.com',
        'www.test2.com', 
        'www.test3.com',
        'www.test4.com',
        'www.test5.com'
    ]
    
    print("   测试相似域名的哈希分布...")
    for domain in similar_domains:
        response_time, success, ip = send_dns_query(domain)
        if response_time:
            print(f"   {domain:20s} -> {response_time:6.2f}ms")
        time.sleep(0.02)

if __name__ == "__main__":
    print(" O(1) LRU Cache 专项测试工具")
    print(" 确保DNS服务器正在运行: ./x64/Debug/DNS.exe -d test_dnsrelay.txt 8.8.8.8")
    print("  按 Ctrl+C 可随时停止测试\n")
    
    try:
        test_o1_lru_cache()
        test_hash_collision()
        
        print(f"\n 测试完成！如果要查看详细缓存统计，请停止DNS服务器查看输出。")
        
    except KeyboardInterrupt:
        print(f"\n\n 测试被用户中断")
    except Exception as e:
        print(f"\n 测试过程中出现错误: {e}")
        print("   请确保DNS服务器正在运行并监听127.0.0.1:53") 