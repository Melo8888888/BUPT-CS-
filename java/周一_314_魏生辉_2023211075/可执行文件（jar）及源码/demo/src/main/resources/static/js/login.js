$(document).ready(function() {
    // 处理游客登录按钮点击
    $('#guestLoginBtn').click(function() {
        // 重定向到主应用页面
        window.location.href = '/guest-login';
    });

    // 处理标准登录表单提交
    $('#loginForm').submit(function(e) {
        e.preventDefault(); // 阻止默认表单提交
        
        // 表单验证
        let isValid = true;
        
        const username = $('#username').val();
        if (username.trim() === '') {
            $('#username').addClass('is-invalid');
            isValid = false;
        }
        
        const password = $('#password').val();
        if (password === '') {
            $('#password').addClass('is-invalid');
            isValid = false;
        }
        
        if (isValid) {
            // 在实际应用中，这里应该发送AJAX请求到后端认证API
            // 这里仅作为演示，显示一个成功消息并延迟跳转
            
            // 禁用表单
            $('#loginForm input, #loginForm button, #guestLoginBtn').prop('disabled', true);
            
            // 创建成功消息
            const successMessage = $('<div class="alert alert-success mb-3" role="alert">')
                .html('<i class="fas fa-check-circle me-2"></i>登录成功！正在跳转...');
            
            // 显示成功消息
            $('#loginForm').prepend(successMessage);
            
            // 2秒后跳转到首页
            setTimeout(function() {
                window.location.href = '/index';
            }, 2000);
        }
    });
    
    // 移除输入框的错误状态
    $('input').on('focus', function() {
        $(this).removeClass('is-invalid');
    });
    
    // 添加一些动画效果
    $('.login-container').css('opacity', '0').animate({
        opacity: 1,
        marginTop: 0
    }, 800);
}); 