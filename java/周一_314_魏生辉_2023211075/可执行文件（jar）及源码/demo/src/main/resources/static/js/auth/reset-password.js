$(document).ready(function() {
    // 密码强度检测
    $('#password').on('input', function() {
        const password = $(this).val();
        updatePasswordStrength(password);
    });

    // 确认密码匹配检测
    $('#confirmPassword').on('input', function() {
        const password = $('#password').val();
        const confirmPassword = $(this).val();
        
        if (confirmPassword === '') {
            $('#passwordMatchFeedback').text('');
        } else if (password === confirmPassword) {
            $('#passwordMatchFeedback').text('密码匹配').css('color', '#28a745');
        } else {
            $('#passwordMatchFeedback').text('密码不匹配').css('color', '#dc3545');
        }
    });

    // 表单提交
    $('#resetPasswordForm').submit(function(e) {
        e.preventDefault();
        
        // 表单验证
        let isValid = true;
        
        const password = $('#password').val();
        if (password.length < 6) {
            $('#password').addClass('is-invalid');
            isValid = false;
        }
        
        const confirmPassword = $('#confirmPassword').val();
        if (password !== confirmPassword) {
            $('#confirmPassword').addClass('is-invalid');
            isValid = false;
        }
        
        if (isValid) {
            // 在实际应用中，这里应该发送AJAX请求到后端API
            // 这里仅作为演示，显示成功消息
            
            // 禁用表单
            $('#resetPasswordForm input, #resetPasswordForm button').prop('disabled', true);
            
            // 模拟API请求延迟
            setTimeout(function() {
                // 隐藏表单
                $('#resetPasswordForm').slideUp();
                
                // 显示成功消息
                $('#successMessage').fadeIn();
                
                // 修改锁图标动画
                $('.cartoon-lock i').removeClass('fa-lock-open').addClass('fa-lock');
                $('.cartoon-lock').css('animation', 'none');
                setTimeout(function() {
                    $('.cartoon-lock').css('animation', 'pulse 1s infinite');
                }, 100);
            }, 1500);
        }
    });
    
    // 移除输入框的错误状态
    $('input').on('focus', function() {
        $(this).removeClass('is-invalid');
    });
    
    // 添加一些动画效果
    $('.cartoon-lock').hover(
        function() {
            $(this).css('transform', 'scale(1.1)');
        },
        function() {
            $(this).css('transform', 'scale(1)');
        }
    );
});

// 密码强度检测函数
function updatePasswordStrength(password) {
    let strength = 0;
    let feedback = '';
    
    if (password.length === 0) {
        strength = 0;
        feedback = '密码强度：未输入';
    } else if (password.length < 6) {
        strength = 1;
        feedback = '密码强度：弱';
    } else {
        strength = 2;
        
        // 检查是否包含数字
        if (/\d/.test(password)) {
            strength++;
        }
        
        // 检查是否包含小写字母
        if (/[a-z]/.test(password)) {
            strength++;
        }
        
        // 检查是否包含大写字母
        if (/[A-Z]/.test(password)) {
            strength++;
        }
        
        // 检查是否包含特殊字符
        if (/[^a-zA-Z0-9]/.test(password)) {
            strength++;
        }
        
        // 根据强度设置反馈信息
        if (strength < 4) {
            feedback = '密码强度：中';
        } else if (strength < 6) {
            feedback = '密码强度：强';
        } else {
            feedback = '密码强度：非常强';
        }
    }
    
    // 更新密码强度条
    const percentage = (strength / 6) * 100;
    $('#passwordStrengthBar').css('width', percentage + '%');
    
    // 设置颜色
    let color;
    if (strength === 0) {
        color = '#e9ecef';
    } else if (strength < 3) {
        color = '#dc3545'; // 红色 - 弱
    } else if (strength < 5) {
        color = '#ffc107'; // 黄色 - 中
    } else {
        color = '#28a745'; // 绿色 - 强
    }
    
    $('#passwordStrengthBar').css('background-color', color);
    $('#passwordFeedback').text(feedback).css('color', color);
} 