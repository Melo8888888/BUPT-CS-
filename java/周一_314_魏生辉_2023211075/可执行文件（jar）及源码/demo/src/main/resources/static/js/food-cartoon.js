// 美食页面卡通动画效果
document.addEventListener('DOMContentLoaded', function() {
    // 添加装饰元素
    addFoodDecorations();
    
    // 初始化卡通动画
    initCartoonAnimations();
    
    // 添加交互效果
    addInteractionEffects();
    
    // 监听筛选和排序事件
    setupFilterListeners();
});

// 添加食物装饰元素
function addFoodDecorations() {
    const container = document.querySelector('.container');
    
    // 添加卡通食物图标装饰
    const foodIcons = [
        { icon: 'fa-hamburger', color: '#EF476F', top: '120px', right: '5%', size: '40px', animation: 'float 6s ease-in-out infinite' },
        { icon: 'fa-pizza-slice', color: '#FFD166', bottom: '30%', left: '5%', size: '35px', animation: 'float 8s ease-in-out infinite' },
        { icon: 'fa-ice-cream', color: '#06D6A0', top: '40%', right: '3%', size: '30px', animation: 'float 7s ease-in-out infinite' },
        { icon: 'fa-coffee', color: '#118AB2', bottom: '20%', right: '8%', size: '25px', animation: 'float 5s ease-in-out infinite' }
    ];
    
    foodIcons.forEach((item, index) => {
        const decoration = document.createElement('div');
        decoration.className = 'food-decoration';
        decoration.innerHTML = `<i class="fas ${item.icon}" style="color: ${item.color}; font-size: ${item.size};"></i>`;
        decoration.style.position = 'absolute';
        decoration.style.zIndex = '-1';
        
        if (item.top) decoration.style.top = item.top;
        if (item.bottom) decoration.style.bottom = item.bottom;
        if (item.left) decoration.style.left = item.left;
        if (item.right) decoration.style.right = item.right;
        decoration.style.animation = item.animation;
        
        container.appendChild(decoration);
    });
}

// 初始化卡通动画
function initCartoonAnimations() {
    // 给食物卡片添加随机初始旋转角度
    const foodCards = document.querySelectorAll('.food-cartoon-card');
    foodCards.forEach(card => {
        const randomRotation = (Math.random() - 0.5) * 3; // -1.5 到 1.5 度之间的随机旋转
        card.style.transform = `rotate(${randomRotation}deg)`;
    });
    
    // 给评分星星添加闪烁效果
    const ratingStars = document.querySelectorAll('.cartoon-stars');
    ratingStars.forEach(stars => {
        setInterval(() => {
            stars.style.textShadow = '2px 2px 5px rgba(255, 209, 102, 0.8)';
            setTimeout(() => {
                stars.style.textShadow = '1px 1px 0 #2B2D42';
            }, 300);
        }, 3000 + Math.random() * 2000); // 随机间隔时间，避免所有星星同时闪烁
    });
    
    // 添加随机浮动气泡
    createFoodBubbles();
}

// 创建食物气泡
function createFoodBubbles() {
    const container = document.querySelector('.container');
    const bubbleEmojis = ['🍔', '🍕', '🍣', '🍜', '🍦', '🍰', '🍩', '🍗', '🥗', '🍱', '🍲', '🥘'];
    
    setInterval(() => {
        const bubble = document.createElement('div');
        bubble.className = 'food-bubble';
        
        // 随机选择一个食物表情
        const randomEmoji = bubbleEmojis[Math.floor(Math.random() * bubbleEmojis.length)];
        bubble.textContent = randomEmoji;
        
        // 随机位置和大小
        const size = 20 + Math.random() * 30;
        bubble.style.fontSize = `${size}px`;
        bubble.style.position = 'absolute';
        bubble.style.bottom = '-20px';
        bubble.style.left = `${Math.random() * 90 + 5}%`;
        bubble.style.opacity = '0.7';
        bubble.style.zIndex = '-1';
        bubble.style.animation = `floatBubble ${5 + Math.random() * 10}s ease-in-out forwards`;
        
        container.appendChild(bubble);
        
        // 移除气泡
        setTimeout(() => {
            bubble.remove();
        }, 15000);
    }, 3000);
}

// 添加交互效果
function addInteractionEffects() {
    // 卡片悬停效果
    const foodCards = document.querySelectorAll('.food-cartoon-card');
    foodCards.forEach(card => {
        card.addEventListener('mouseenter', function() {
            this.style.transform = 'translateY(-8px) rotate(2deg)';
            this.style.boxShadow = '8px 8px 0 #2B2D42';
            
            // 添加弹跳效果到图标
            const icon = this.querySelector('.cartoon-location');
            if (icon) {
                icon.classList.add('bounce');
            }
        });
        
        card.addEventListener('mouseleave', function() {
            const randomRotation = (Math.random() - 0.5) * 3;
            this.style.transform = `rotate(${randomRotation}deg)`;
            this.style.boxShadow = '5px 5px 0 #2B2D42';
            
            // 移除弹跳效果
            const icon = this.querySelector('.cartoon-location');
            if (icon) {
                icon.classList.remove('bounce');
            }
        });
    });
    
    // 按钮点击效果
    const buttons = document.querySelectorAll('.cartoon-food-btn');
    buttons.forEach(button => {
        button.addEventListener('click', function() {
            this.classList.add('clicked');
            
            setTimeout(() => {
                this.classList.remove('clicked');
            }, 300);
        });
    });
    
    // 筛选项悬停效果
    const checkboxLabels = document.querySelectorAll('.cartoon-checkbox');
    checkboxLabels.forEach(label => {
        label.addEventListener('mouseenter', function() {
            this.style.transform = 'translateX(5px)';
        });
        
        label.addEventListener('mouseleave', function() {
            this.style.transform = 'translateX(0)';
        });
    });
}

// 设置筛选监听器
function setupFilterListeners() {
    const applyFiltersBtn = document.getElementById('applyFiltersBtn');
    if (applyFiltersBtn) {
        applyFiltersBtn.addEventListener('click', function() {
            // 添加按钮点击动画
            this.classList.add('pulse');
            setTimeout(() => {
                this.classList.remove('pulse');
            }, 500);
            
            // 添加筛选应用效果
            const foodCards = document.querySelectorAll('.food-cartoon-card');
            foodCards.forEach(card => {
                card.style.opacity = '0.5';
                setTimeout(() => {
                    card.style.opacity = '1';
                }, 500);
            });
        });
    }
}

// 添加CSS样式
const style = document.createElement('style');
style.textContent = `
    @keyframes floatBubble {
        0% {
            transform: translateY(0) scale(1) rotate(0);
            opacity: 0.7;
        }
        50% {
            opacity: 0.9;
        }
        100% {
            transform: translateY(-1000px) scale(0.5) rotate(360deg);
            opacity: 0;
        }
    }
    
    .food-bubble {
        pointer-events: none;
    }
    
    .clicked {
        transform: translateY(2px) !important;
        box-shadow: 0 1px 0 #2B2D42 !important;
    }
    
    .cartoon-checkbox {
        transition: transform 0.3s ease;
    }
`;
document.head.appendChild(style); 