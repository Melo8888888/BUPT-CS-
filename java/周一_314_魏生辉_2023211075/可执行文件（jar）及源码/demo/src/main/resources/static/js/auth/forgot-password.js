$(document).ready(function() {
    // 表单提交
    $('#forgotPasswordForm').submit(function(e) {
        e.preventDefault();
        
        // 表单验证
        const email = $('#email').val();
        if (!isValidEmail(email)) {
            $('#email').addClass('is-invalid');
            return;
        }
        
        // 在实际应用中，这里应该发送AJAX请求到后端API
        // 这里仅作为演示，显示成功消息
        
        // 禁用表单
        $('#forgotPasswordForm input, #forgotPasswordForm button').prop('disabled', true);
        
        // 模拟API请求延迟
        setTimeout(function() {
            // 显示成功消息
            $('#successMessage').fadeIn();
            
            // 隐藏表单
            $('#forgotPasswordForm').slideUp();
        }, 1500);
    });
    
    // 移除输入框的错误状态
    $('#email').on('focus', function() {
        $(this).removeClass('is-invalid');
    });
    
    // 添加一些动画效果
    $('.cartoon-envelope').hover(
        function() {
            $(this).css('transform', 'scale(1.1)');
        },
        function() {
            $(this).css('transform', 'scale(1)');
        }
    );
});

// 邮箱验证函数
function isValidEmail(email) {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
} 