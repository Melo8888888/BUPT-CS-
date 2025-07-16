/**
 * 美食推荐模块测试脚本
 * 用于测试前端展示和API调用功能（仅后台调试使用）
 */
$(document).ready(function() {
    console.log("Food Test JS loaded successfully");
    
    // 检查页面是否已加载美食数据
    testFoodDataLoading();
    
    // 不再向前端添加测试信息
    // addTestFoodCard();
    // addTestInfoPanel();
});

/**
 * 检查页面是否已加载美食数据
 */
function testFoodDataLoading() {
    const foodItems = $('.food-card').length;
    console.log(`Detected ${foodItems} food items on the page`);
    
    // 如果没有美食项，尝试从API获取数据
    if (foodItems === 0) {
        console.log("No food items found, attempting to fetch from API...");
        fetchTestFoodData();
    }
}

/**
 * 从API获取测试美食数据
 */
function fetchTestFoodData() {
    $.ajax({
        url: '/api/food-recommendations/test',
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
 * 添加测试美食卡片
 */
function addTestFoodCard() {
    const testCard = `
        <div class="col-md-4 mb-4">
            <div class="card food-card h-100" data-id="test" data-cuisine="test">
                <div class="card-body">
                    <h5 class="card-title">测试美食卡片</h5>
                    <span class="badge bg-primary">测试</span>
                    <div class="mb-2 rating-stars">
                        <i class="fas fa-star"></i>
                        <i class="fas fa-star"></i>
                        <i class="fas fa-star"></i>
                        <i class="fas fa-star"></i>
                        <i class="fas fa-star"></i>
                        <span class="rating-value">5.0</span>
                    </div>
                    <p class="card-text text-truncate">
                        <span>测试位置</span>
                    </p>
                    <p class="card-text description-text">这是一个测试美食卡片，用于验证前端渲染功能是否正常工作。</p>
                </div>
                <div class="card-footer">
                    <button class="btn btn-sm btn-outline-primary view-details-btn">查看详情</button>
                </div>
            </div>
        </div>
    `;
    
    // 将测试卡片添加到列表中
    $('.food-list .row').append(testCard);
    console.log("Test food card added");
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
                    <p>美食测试模块已加载</p>
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