// 等待DOM加载完成
document.addEventListener('DOMContentLoaded', function() {
    // 初始化AOS动画库
    if (typeof AOS !== 'undefined') {
        AOS.init({
            duration: 800,
            easing: 'ease-out-cubic',
            once: true
        });
    }
    
    // 添加卡片悬停效果
    const featureCards = document.querySelectorAll('.feature-card');
    featureCards.forEach(card => {
        card.addEventListener('mouseenter', function() {
            this.style.transform = 'translateY(-8px) rotate(2deg)';
        });
        card.addEventListener('mouseleave', function() {
            this.style.transform = 'translateY(0) rotate(0deg)';
        });
    });
    
    // 导航栏交互
    const navLinks = document.querySelectorAll('.nav-link');
    navLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            navLinks.forEach(l => l.classList.remove('active'));
            this.classList.add('active');
        });
    });
    
    // 底部导航交互
    const footerNavItems = document.querySelectorAll('.footer-nav-item');
    footerNavItems.forEach(item => {
        item.addEventListener('click', function(e) {
            footerNavItems.forEach(i => i.classList.remove('active'));
            this.classList.add('active');
        });
    });

    // 按钮点击效果
    const buttons = document.querySelectorAll('.cartoon-btn');
    buttons.forEach(button => {
        button.addEventListener('click', function() {
            // 添加点击波纹效果
            const ripple = document.createElement('span');
            ripple.classList.add('ripple-effect');
            
            this.appendChild(ripple);
            
            const rect = this.getBoundingClientRect();
            const size = Math.max(rect.width, rect.height);
            
            ripple.style.width = ripple.style.height = `${size}px`;
            ripple.style.left = `${event.clientX - rect.left - size / 2}px`;
            ripple.style.top = `${event.clientY - rect.top - size / 2}px`;
            
            ripple.classList.add('active');
            
            // 添加按钮动画
            this.classList.add('clicked');
            
            setTimeout(() => {
                ripple.remove();
                this.classList.remove('clicked');
            }, 600);
        });
    });

    // 添加页面滚动效果
    window.addEventListener('scroll', function() {
        const header = document.querySelector('.header');
        if (window.scrollY > 50) {
            header.classList.add('header-scrolled');
        } else {
            header.classList.remove('header-scrolled');
        }
    });

    // 添加搜索按钮功能
    const searchBtn = document.querySelector('.header-icon-btn:first-child');
    if (searchBtn) {
        searchBtn.addEventListener('click', function() {
            alert('搜索功能即将上线！');
        });
    }
    
    // 随机移动卡通装饰元素
    animateCartoonElements();
});

// 随机移动卡通装饰元素
function animateCartoonElements() {
    const clouds = document.querySelectorAll('.cartoon-cloud');
    const stars = document.querySelectorAll('.cartoon-star');
    
    // 云朵随机移动
    clouds.forEach(cloud => {
        setInterval(() => {
            const xPos = Math.random() * 20 - 10; // -10px 到 10px
            const duration = 5 + Math.random() * 5; // 5-10秒
            
            cloud.style.transition = `transform ${duration}s ease-in-out`;
            cloud.style.transform = `translateX(${xPos}px) scale(${0.7 + Math.random() * 0.3})`;
            
            setTimeout(() => {
                cloud.style.transform = 'translateX(0) scale(1)';
            }, duration * 1000);
        }, 10000);
    });
    
    // 星星闪烁
    stars.forEach(star => {
        setInterval(() => {
            star.style.opacity = '0.3';
            
            setTimeout(() => {
                star.style.opacity = '1';
            }, 300);
        }, 3000);
    });
    
    // 创建随机漂浮的气泡
    createRandomBubbles();
}

// 创建随机漂浮的气泡
function createRandomBubbles() {
    const mainContent = document.querySelector('.main-content');
    const colors = ['#FFD166', '#06D6A0', '#118AB2', '#EF476F'];
    
    setInterval(() => {
        const bubble = document.createElement('div');
        bubble.className = 'random-bubble';
        
        const size = 10 + Math.random() * 30;
        const color = colors[Math.floor(Math.random() * colors.length)];
        
        bubble.style.width = `${size}px`;
        bubble.style.height = `${size}px`;
        bubble.style.backgroundColor = color;
        bubble.style.position = 'absolute';
        bubble.style.borderRadius = '50%';
        bubble.style.opacity = '0.6';
        bubble.style.zIndex = '-1';
        bubble.style.left = `${Math.random() * 100}%`;
        bubble.style.bottom = '-20px';
        bubble.style.animation = `floatBubble ${5 + Math.random() * 10}s linear forwards`;
        
        mainContent.appendChild(bubble);
        
        // 移除气泡
        setTimeout(() => {
            bubble.remove();
        }, 15000);
    }, 3000);
}

// 添加CSS样式
const style = document.createElement('style');
style.textContent = `
    .ripple-effect {
        position: absolute;
        border-radius: 50%;
        background-color: rgba(255, 255, 255, 0.4);
        transform: scale(0);
        animation: ripple 0.6s linear;
        pointer-events: none;
    }

    @keyframes ripple {
        to {
            transform: scale(2);
            opacity: 0;
        }
    }

    .header-scrolled {
        box-shadow: var(--shadow-md);
        background-color: rgba(255, 255, 255, 0.95);
        backdrop-filter: blur(10px);
        -webkit-backdrop-filter: blur(10px);
    }

    .footer-nav-item.active {
        color: var(--primary-color);
        font-weight: 500;
    }

    .footer-nav-item.active i {
        transform: translateY(-3px);
    }
    
    .cartoon-btn.clicked {
        transform: translateY(4px);
        box-shadow: 0 0 0 #2B2D42;
    }
    
    @keyframes floatBubble {
        0% {
            transform: translateY(0) scale(1) rotate(0);
            opacity: 0.6;
        }
        50% {
            opacity: 0.8;
        }
        100% {
            transform: translateY(-1000px) scale(0) rotate(360deg);
            opacity: 0;
        }
    }
    
    .random-bubble {
        pointer-events: none;
    }
`;
document.head.appendChild(style); 