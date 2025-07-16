package com.example.demo.study.config;

import java.time.LocalDateTime;
import java.util.Arrays;
import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.core.annotation.Order;

import com.example.demo.study.model.StudyNote;
import com.example.demo.study.repository.StudyNoteRepository;

@Configuration
public class StudyDataInitializer {

    @Autowired
    private StudyNoteRepository studyNoteRepository;

    @Bean
    @Order(1)
    public CommandLineRunner initStudyData() {
        return args -> {
            try {
                // 判断数据库中是否已有笔记数据
                long count = studyNoteRepository.count();
                System.out.println("====================================");
                System.out.println("检查学习笔记数据: 当前数量 = " + count);

                if (count == 0) {
                    System.out.println("初始化学习笔记示例数据...");
                    initializeSampleNotes();
                } else {
                    System.out.println("数据库中已有学习笔记数据，跳过初始化。");
                    System.out.println("当前笔记总数：" + count);
                }
                System.out.println("====================================");
            } catch (Exception e) {
                System.err.println("初始化学习笔记数据失败:");
                e.printStackTrace();
            }
        };
    }

    private void initializeSampleNotes() {
        List<StudyNote> sampleNotes = Arrays.asList(
                createNote(
                        "JavaScript 基础知识",
                        "programming",
                        "# JavaScript 基础\n\n"
                        + "JavaScript 是一种具有函数优先特性的轻量级解释型或者JIT编译型的编程语言。\n\n"
                        + "## 变量声明\n"
                        + "```javascript\n"
                        + "// 使用 let 声明变量\n"
                        + "let message = 'Hello';\n\n"
                        + "// 使用 const 声明常量\n"
                        + "const PI = 3.14159;\n"
                        + "```\n\n"
                        + "## 数据类型\n"
                        + "JavaScript 有七种基本数据类型：\n"
                        + "- String\n"
                        + "- Number\n"
                        + "- Boolean\n"
                        + "- null\n"
                        + "- undefined\n"
                        + "- Symbol (ES6)\n"
                        + "- BigInt (ES2020)"
                ),
                createNote(
                        "英语常用词汇",
                        "language",
                        "# 英语常用词汇\n\n"
                        + "以下是一些常用的英语词汇及其中文释义：\n\n"
                        + "1. **Adapt** - 适应，调整\n"
                        + "2. **Brief** - 简短的，简洁的\n"
                        + "3. **Crucial** - 至关重要的，决定性的\n"
                        + "4. **Diverse** - 多样的，不同的\n"
                        + "5. **Enhance** - 提高，增强\n"
                        + "6. **Feasible** - 可行的，可能的\n"
                        + "7. **Generate** - 产生，生成\n"
                        + "8. **Hierarchy** - 层级，等级制度\n"
                        + "9. **Impact** - 影响，冲击\n"
                        + "10. **Justify** - 证明...是正确的\n\n"
                        + "建议每天学习5-10个新词，并尝试在日常交流中使用它们。"
                ),
                createNote(
                        "微积分公式大全",
                        "math",
                        "# 常见微积分公式\n\n"
                        + "## 导数公式\n\n"
                        + "1. $(c)' = 0$ (常数的导数)\n"
                        + "2. $(x^n)' = nx^{n-1}$ (幂函数的导数)\n"
                        + "3. $(sin(x))' = cos(x)$ (正弦函数的导数)\n"
                        + "4. $(cos(x))' = -sin(x)$ (余弦函数的导数)\n"
                        + "5. $(e^x)' = e^x$ (指数函数的导数)\n"
                        + "6. $(ln(x))' = \\frac{1}{x}$ (自然对数的导数)\n\n"
                        + "## 积分公式\n\n"
                        + "1. $\\int c \\, dx = cx + C$ (常数的积分)\n"
                        + "2. $\\int x^n \\, dx = \\frac{x^{n+1}}{n+1} + C$ (幂函数的积分，$n \\neq -1$)\n"
                        + "3. $\\int sin(x) \\, dx = -cos(x) + C$ (正弦函数的积分)\n"
                        + "4. $\\int cos(x) \\, dx = sin(x) + C$ (余弦函数的积分)\n"
                        + "5. $\\int e^x \\, dx = e^x + C$ (指数函数的积分)\n"
                        + "6. $\\int \\frac{1}{x} \\, dx = ln|x| + C$ (倒数函数的积分)"
                )
        );

        studyNoteRepository.saveAll(sampleNotes);
        System.out.println("已创建 " + sampleNotes.size() + " 条示例笔记");
    }

    private StudyNote createNote(String title, String category, String content) {
        StudyNote note = new StudyNote();
        note.setTitle(title);
        note.setCategory(category);
        note.setContent(content);
        note.setCreatedAt(LocalDateTime.now());
        note.setUpdatedAt(LocalDateTime.now());
        return note;
    }
}
