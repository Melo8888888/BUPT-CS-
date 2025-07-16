/**
 * 穿搭推荐模块测试脚本
 * 用于测试前端展示和筛选功能（仅后台调试使用）
 */
$(document).ready(function() {
    console.log("Fashion Test JS loaded successfully");
    
    // 检查页面是否已加载穿搭数据
    testOutfitDataLoading();
    
    // 不再向前端添加测试信息
    // addTestOutfitCard();
    // addTestInfoPanel();
});

/**
 * 检查页面是否已加载穿搭数据
 */
function testOutfitDataLoading() {
    const outfitItems = $('.outfit-item').length;
    console.log(`Detected ${outfitItems} outfit items on the page`);
    
    // 如果没有穿搭项，尝试从API获取数据
    if (outfitItems === 0) {
        console.log("No outfit items found, attempting to fetch from API...");
        fetchTestOutfitData();
    }
}

/**
 * 从API获取测试穿搭数据
 */
function fetchTestOutfitData() {
    $.ajax({
        url: '/api/fashion-outfits/test',
        method: 'GET',
        success: function(response) {
            console.log("API test response:", response);
            // 不再显示测试面板
            // updateTestInfo("API响应成功", "success", JSON.stringify(response));
        },
        error: function(xhr, status, error) {
            console.error("API test error:", error);
            // 不再显示测试面板
            // updateTestInfo("API响应失败", "danger", `错误: ${error}, 状态: ${status}`);
        }
    });
}

// 保留这些函数但不在初始化时调用，仅供开发调试

/**
 * 添加测试穿搭卡片
 */
function addTestOutfitCard() {
    const testCard = `
        <div class="col-md-6 col-lg-4 mb-4 outfit-item test-outfit" data-occasion="casual" data-style="streetwear">
            <div class="card outfit-card h-100" data-occasion="casual">
                <div class="outfit-style-badge">
                    <span class="badge style-streetwear">街头</span>
                </div>
                <div class="card-body">
                    <h5 class="card-title">测试穿搭卡片</h5>
                    <div class="mb-3">
                        <span class="badge bg-secondary occasion-badge-casual">休闲</span>
                    </div>
                    <p class="card-text description-text">这是一个测试穿搭卡片，用于验证前端渲染功能是否正常工作。</p>
                    <div class="outfit-items">
                        <p>上衣：测试T恤</p>
                        <p>裤子：测试牛仔裤</p>
                        <p>鞋子：测试运动鞋</p>
                    </div>
                </div>
                <div class="card-footer bg-white border-0">
                    <button class="btn btn-sm btn-outline-primary w-100 view-details-btn">查看详情</button>
                </div>
            </div>
        </div>
    `;
    
    // 将测试卡片添加到列表中
    $('#fashionOutfitsList').append(testCard);
    console.log("Test outfit card added");
    
    // 为新添加的测试卡片绑定点击事件
    $('.test-outfit .view-details-btn').click(function() {
        const $card = $(this).closest('.outfit-card');
        const title = $card.find('.card-title').text();
        const description = $card.find('.description-text').text();
        const occasion = $card.data('occasion');
        const style = $card.parent().data('style');
        const items = $card.find('.outfit-items').html();
        
        // 将数据填充到模态框
        $('#viewOutfitTitle').text(title);
        $('#viewOutfitDescription').text(description);
        
        // 设置场合标签
        let occasionText = '';
        switch(occasion) {
            case 'casual': occasionText = '休闲'; break;
            case 'formal': occasionText = '正式'; break;
            case 'party': occasionText = '派对'; break;
            case 'work': occasionText = '工作'; break;
        }
        $('#viewOutfitOccasion').text(occasionText);
        $('#viewOutfitOccasion').attr('class', 'badge bg-secondary occasion-badge-' + occasion);
        
        // 设置风格标签
        let styleText = '';
        switch(style) {
            case 'minimalist': styleText = '极简'; break;
            case 'streetwear': styleText = '街头'; break;
            case 'vintage': styleText = '复古'; break;
            case 'korean': styleText = '韩风'; break;
            case 'sporty': styleText = '运动'; break;
            case 'business': styleText = '商务'; break;
        }
        $('#viewOutfitStyle').text(styleText);
        $('#viewOutfitStyle').attr('class', 'badge style-' + style);
        
        // 填充单品列表
        $('#viewOutfitItems').html(items);
        
        // 显示模态框
        $('#viewOutfitModal').modal('show');
    });
}

/**
 * 添加测试信息面板
 */
function addTestInfoPanel() {
    const testPanel = `
        <div id="testInfoPanel" style="position: fixed; bottom: 20px; right: 20px; width: 300px; z-index: 9999; display: none;">
            <div class="card">
                <div class="card-header bg-info text-white d-flex justify-content-between align-items-center">
                    <span>测试信息</span>
                    <button type="button" class="btn-close btn-close-white" aria-label="Close" id="closeTestInfo"></button>
                </div>
                <div class="card-body" id="testInfoContent">
                    <p>穿搭测试模块已加载</p>
                </div>
            </div>
        </div>
    `;
    
    $('body').append(testPanel);
    $('#testInfoPanel').fadeIn();
    
    // 绑定关闭按钮事件
    $('#closeTestInfo').click(function() {
        $('#testInfoPanel').fadeOut();
    });
}

/**
 * 更新测试信息面板内容
 */
function updateTestInfo(title, type, content) {
    const typeClass = type === 'success' ? 'bg-success' : 'bg-danger';
    $('#testInfoPanel .card-header').removeClass('bg-info bg-success bg-danger').addClass(typeClass);
    
    let contentHtml = `<h6>${title}</h6>`;
    if (content) {
        contentHtml += `<div class="mt-2 p-2 bg-light rounded"><pre style="margin-bottom: 0; font-size: 0.8rem; max-height: 200px; overflow-y: auto;">${content}</pre></div>`;
    }
    
    $('#testInfoContent').html(contentHtml);
    $('#testInfoPanel').fadeIn();
} 