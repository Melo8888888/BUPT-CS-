/**
 * 行程路线规划模块
 * 负责前端路线查询和高德地图交互
 */

// 全局变量
let map = null;
let markers = [];
let transportType = 'walking';
let isUsingMockApi = false; // 设置为false，直接使用高德API而非模拟API
let isMapReady = false; // 地图API是否已加载完成

// 默认起点坐标（北京邮电大学）
const BUPT_LOCATION = [116.361303, 39.963387];
const DEFAULT_ORIGIN = "北京邮电大学(西土城路校区)";

// 预定义的热门目的地坐标
const POPULAR_DESTINATIONS = {
    "北京西站": [116.322056, 39.894703],
    "天安门广场": [116.397755, 39.908702],
    "故宫博物院": [116.397026, 39.918058],
    "北京大学": [116.310902, 39.992806],
    "798艺术区": [116.49596, 39.984079],
    "王府井步行街": [116.418427, 39.915205],
    "三里屯太古里": [116.454563, 39.938608],
    "中国国家博物馆": [116.401769, 39.905293],
    "颐和园": [116.2755, 39.9988],
    "圆明园": [116.3032, 40.0088],
    "鸟巢(国家体育场)": [116.3975, 39.9926],
    "水立方(国家游泳中心)": [116.3907, 39.9932],
    "清华大学": [116.3266, 40.0032],
    "北京动物园": [116.3386, 39.9409],
    "长城(八达岭)": [116.0166, 40.3655],
    "世界公园": [116.1749, 39.8168],
    "朝阳公园": [116.4816, 39.9327],
    "什刹海": [116.3877, 39.9401],
    "南锣鼓巷": [116.4032, 39.9392],
    "后海": [116.3817, 39.9441],
    "北海公园": [116.3897, 39.9263],
    "香山公园": [116.1849, 39.9996],
    "首都博物馆": [116.3372, 39.9064],
    "北京欢乐谷": [116.4931, 39.8743],
    "天坛公园": [116.4116, 39.8822]
};

// 定义全局初始化函数，供高德地图API加载完成后调用
window.initMapApplication = function() {
    console.log('应用初始化开始...');
    isMapReady = true;
    initMap();
    
    // 检查和显示API密钥信息
    console.log("当前高德地图API密钥:", window._AMapSecurityConfig ? "已配置" : "未配置");
    if (typeof AMap !== 'undefined') {
        AMap.plugin('AMap.Map', function() {
            console.log("高德地图API加载成功");
        });
    }
};

// 检查AMap是否已加载
if (typeof AMap !== 'undefined') {
    console.log('高德地图API已加载，直接初始化');
    isMapReady = true;
    $(document).ready(function() {
        initMap();
    });
}

// 页面DOM加载完成后执行
$(document).ready(function() {
    console.log('页面DOM已加载完成');
    
    // 初始化页面事件
    initPageEvents();
});

/**
 * 初始化页面事件
 */
function initPageEvents() {
    // 交通方式切换
    $('.route-type-btn').click(function() {
        $('.route-type-btn').removeClass('active');
        $(this).addClass('active');
        transportType = $(this).data('type');
    });
    
    // 热门目的地路线按钮点击
    $('.route-btn').click(function() {
        let destination = $(this).data('destination');
        $('#destination').val(destination);
        searchRoute();
    });
    
    // 热门目的地选择按钮点击
    $('.destination-select-btn').click(function() {
        let destination = $(this).data('destination');
        $('#destination').val(destination).trigger('change');
        searchRoute();
    });
    
    // 表单提交
    $('#routePlanForm').submit(function(e) {
        e.preventDefault();
        searchRoute();
    });
    
    // 下拉框变化时更新界面
    $('#destination').change(function() {
        let selected = $(this).val();
        if (selected) {
            // 高亮对应的快速选择按钮
            $('.destination-select-btn').removeClass('active');
            $('.destination-select-btn[data-destination="' + selected + '"]').addClass('active');
        }
    });

    // AI Recommendation Logic
    const aiRecommendations = [
        '今天适合去 **城市绿洲公园**，这里风景宜人，适合漫步，享受大自然的清新空气，或许还能遇到可爱的松鼠。带上你的相机，捕捉美好瞬间，期待您的探索！',
        '推荐您前往 **历史文化街区**，感受老北京的独特韵味，品尝地道小吃，沉浸在时光的魅力中。别忘了带上您的探索精神！',
        '不妨考虑去 **创意艺术园区**，那里有最新的展览和独特的艺术作品，激发您的灵感，体验城市的时尚与活力。用艺术充实您的一天！',
        '今天的天气非常适合去 **近郊山区**，远离喧嚣，亲近自然，享受徒步的乐趣和山间的宁静。别忘了带上舒适的鞋子！',
        '推荐您参观 **科技博物馆**，探索前沿科技的奥秘，体验互动装置，让知识和乐趣同行。未来已来，一起去看看吧！'
    ];

    $('#aiRecommendationBtn').click(function() {
        const randomIndex = Math.floor(Math.random() * aiRecommendations.length);
        const recommendation = aiRecommendations[randomIndex];
        $('#aiRecommendationText').html(recommendation);
    });
}

/**
 * 搜索路线
 */
function searchRoute() {
    const destination = $('#destination').val();
    
    if (!destination) {
        showToast('请选择目的地');
        return;
    }
    
    // 显示加载指示器
    showLoading();
    
    // 显示结果容器
    $('#routeResultContainer').show();
    
    // 使用服务器API规划路线
    useServerApi(destination);
}

/**
 * 使用服务器API进行路线规划
 */
function useServerApi(destination) {
    // 获取目的地坐标
    let destCoords;
    
    // 检查是否是预定义的目的地
    if (POPULAR_DESTINATIONS[destination]) {
        destCoords = POPULAR_DESTINATIONS[destination];
        console.log('使用预定义坐标:', destination, destCoords);
        
        // 直接规划路线
        directPlanRoute(BUPT_LOCATION, destCoords, destination);
    } else {
        // 使用地理编码服务获取坐标
        $.ajax({
            url: '/api/travel/geocode',
            method: 'GET',
            data: {
                address: destination
            },
            success: function(response) {
                if (response && response.status === '1' && response.geocodes && response.geocodes.length > 0) {
                    const location = response.geocodes[0].location.split(',');
                    destCoords = [parseFloat(location[0]), parseFloat(location[1])];
                    
                    // 规划路线
                    directPlanRoute(BUPT_LOCATION, destCoords, destination);
                } else {
                    hideLoading();
                    showToast('无法解析目的地地址，请尝试输入更详细的地址');
                }
            },
            error: function(error) {
                hideLoading();
                showToast('地址解析失败，请稍后重试');
                console.error('地址解析失败:', error);
            }
        });
    }
}

/**
 * 直接规划路线
 */
function directPlanRoute(origin, destination, destinationName) {
    // 清除之前的路线
    clearMarkers();
    
    // 添加起点和终点标记
    addMarker(origin, DEFAULT_ORIGIN);
    addMarker(destination, destinationName || "目的地");
    
    // 调整地图视野以包含所有标记点
    map.setFitView();
    
    // 从服务器获取路线数据
    const originStr = Array.isArray(origin) ? origin[0] + ',' + origin[1] : origin;
    const destinationStr = Array.isArray(destination) ? destination[0] + ',' + destination[1] : destination;
    
    let apiEndpoint;
    switch (transportType) {
        case 'walking':
            apiEndpoint = '/api/travel/walking-route';
            break;
        case 'driving':
            apiEndpoint = '/api/travel/driving-route';
            break;
        case 'transit':
            apiEndpoint = '/api/travel/transit-route';
            break;
        case 'bicycling':
            apiEndpoint = '/api/travel/bicycling-route';
            break;
        default:
            apiEndpoint = '/api/travel/walking-route';
    }
    
    console.log("正在请求路线数据，端点: " + apiEndpoint);
    
    $.ajax({
        url: apiEndpoint,
        method: 'GET',
        data: {
            origin: originStr,
            destination: destinationStr
        },
        success: function(response) {
            hideLoading();
            console.log('路线规划API返回:', response);
            
            // 检查响应状态
            if (response && response.status === '1') {
                // 提取路线信息
                let route = {};
                let path = null;
                
                // 根据不同的交通方式，处理不同的响应结构
                if (transportType === 'walking' || transportType === 'driving') {
                    if (response.route && response.route.paths && response.route.paths.length > 0) {
                        path = response.route.paths[0];
                        route = {
                            distance: path.distance,
                            duration: path.duration,
                            steps: path.steps || []
                        };
                        
                        // 在地图上绘制实际路线
                        drawPathOnMap(path, origin, destination);
                        
                        // 更新路线信息
                        renderRouteInfo(route);
                        return;
                    }
                } else if (transportType === 'transit') {
                    if (response.route && response.route.transits && response.route.transits.length > 0) {
                        path = response.route.transits[0];
                        route = {
                            distance: path.distance,
                            duration: path.duration,
                            segments: path.segments || []
                        };
                        
                        // 尝试绘制公交路线
                        drawTransitPath(path, origin, destination);
                        
                        // 更新路线信息
                        renderRouteInfo(route);
                        return;
                    }
                } else if (transportType === 'bicycling') {
                    if (response.data && response.data.paths && response.data.paths.length > 0) {
                        path = response.data.paths[0];
                        route = {
                            distance: path.distance,
                            duration: path.duration,
                            steps: path.steps || []
                        };
                        
                        // 在地图上绘制骑行路线
                        drawBicyclingPath(path, origin, destination);
                        
                        // 更新路线信息
                        renderRouteInfo(route);
                        return;
                    }
                }
                
                // 如果没有找到有效路径数据，显示简单直线
                console.warn('未找到有效路径数据，显示直线距离');
                drawSimplePath(origin, destination);
            } else {
                // 如果规划失败，显示一条简单的直线
                console.error('路线规划API返回错误状态:', response.info || response.errmsg || '未知错误', response);
                
                // 显示错误信息
                let errorMessage = '无法获取详细路线，显示直线距离';
                if (response && (response.info === 'USERKEY_PLAT_NOMATCH' || response.errmsg === 'USERKEY_PLAT_NOMATCH')) {
                    errorMessage = 'API密钥未授权，请使用有效的高德地图API密钥';
                }
                
                showToast(errorMessage);
                drawSimplePath(origin, destination);
            }
        },
        error: function(error) {
            hideLoading();
            console.error('路线规划请求失败:', error);
            showToast('路线规划请求失败，显示直线距离');
            
            // 绘制简单路径
            drawSimplePath(origin, destination);
        }
    });
}

/**
 * 绘制简单直线路径（无法获取详细路线时的备用方案）
 */
function drawSimplePath(origin, destination) {
    try {
        if (!map) {
            initMap();
        }
        
        // 确保标记已添加
        clearMarkers();
        addMarker(origin, DEFAULT_ORIGIN);
        addMarker(destination, "目的地");
        
        // 创建一条直线
        const polyline = new AMap.Polyline({
            path: [origin, destination],
            isOutline: true,
            outlineColor: '#ffeeee',
            borderWeight: 2,
            strokeWeight: 5,
            strokeColor: '#0091ff',
            lineJoin: 'round',
            strokeStyle: 'dashed',
            strokeDasharray: [5, 5]
        });
        
        map.add(polyline);
        map.setFitView();
        
        // 计算直线距离
        const distance = AMap.GeometryUtil.distance(
            new AMap.LngLat(origin[0], origin[1]),
            new AMap.LngLat(destination[0], destination[1])
        );
        
        // 估算时间（步行速度约为4km/h）
        const speed = transportType === 'walking' ? 4 : 
                      transportType === 'bicycling' ? 12 : 
                      transportType === 'driving' ? 30 : 5;
        
        const duration = Math.ceil((distance / 1000) / speed * 60);
        
        // 更新路线信息
        $('#totalDistance').text((distance / 1000).toFixed(2) + ' 公里（直线距离）');
        $('#totalDuration').text(duration + ' 分钟（估计）');
        
        // 更新路线步骤
        $('#routeSteps').html('<div class="route-step"><p class="route-instruction">无法获取详细路线，显示直线距离</p></div>');
        
    } catch (error) {
        console.error('绘制简单路径错误:', error);
    }
}

/**
 * 初始化地图
 */
function initMap() {
    try {
        if (typeof AMap === 'undefined') {
            console.error('高德地图API尚未加载完成');
            $('#mapContainer').html('<div class="alert alert-warning">地图加载失败，请检查网络连接或刷新页面重试</div>');
            return;
        }
        
        console.log('开始初始化地图...');
        
        // 创建地图实例
        map = new AMap.Map('mapContainer', {
            zoom: 13,
            center: BUPT_LOCATION,
            resizeEnable: true
        });
        
        console.log('地图初始化成功');
        
        // 添加起点标记
        addMarker(BUPT_LOCATION, DEFAULT_ORIGIN);
        
        // 添加一些预定义目的地标记（可选）
        addPopularDestinationMarkers();
        
    } catch (error) {
        console.error('Error initializing map:', error);
        $('#mapContainer').html('<div class="alert alert-warning">地图加载失败，错误信息: ' + error.message + '</div>');
    }
}

/**
 * 添加标记点
 */
function addMarker(position, title) {
    try {
        if (typeof AMap === 'undefined' || !map) {
            console.error('地图未初始化，无法添加标记');
            return null;
        }
        
        const marker = new AMap.Marker({
            position: position,
            map: map,
            title: title
        });
        markers.push(marker);
        return marker;
    } catch (error) {
        console.error('Error adding marker:', error);
        return null;
    }
}

/**
 * 清除所有标记点和路线
 */
function clearMarkers() {
    // 清除所有标记点
    for (let i = 0; i < markers.length; i++) {
        if (markers[i]) {
            markers[i].setMap(null);
        }
    }
    markers = [];
    
    // 清除地图上的所有覆盖物（包括路线）
    if (map) {
        map.clearMap();
    }
}

/**
 * 地理编码（地址转坐标）
 */
function geocodeAddress(address) {
    try {
        AMap.plugin('AMap.Geocoder', function() {
            const geocoder = new AMap.Geocoder();
            
            geocoder.getLocation(address, function(status, result) {
                if (status === 'complete' && result.info === 'OK') {
                    const location = [
                        result.geocodes[0].location.lng,
                        result.geocodes[0].location.lat
                    ];
                    
                    // 添加目的地标记
                    addMarker(location, address);
                    
                    // 根据交通方式选择不同的路线规划方法
                    planRoute(BUPT_LOCATION, location, address);
                    
                } else {
                    hideLoading();
                    showToast('地址解析失败，请输入有效的地址');
                }
            });
        });
    } catch (error) {
        hideLoading();
        console.error('Error geocoding address:', error);
        showToast('地址解析出错，请稍后重试');
    }
}

/**
 * 模拟在地图上绘制路线
 */
function simulateDrawRouteOnMap(origin, destination) {
    try {
        if (typeof AMap === 'undefined' || !map) {
            console.error('地图未初始化，无法绘制路线');
            return;
        }
        
        // 清除现有标记
        clearMarkers();
        
        // 添加起点标记
        addMarker(BUPT_LOCATION, origin);
        
        // 模拟目的地坐标（实际应用中应该通过地理编码获取）
        // 这里只是随机生成一个北邮附近的坐标用于演示
        const destinationLocation = [
            BUPT_LOCATION[0] + (Math.random() * 0.02 - 0.01),
            BUPT_LOCATION[1] + (Math.random() * 0.02 - 0.01)
        ];
        
        // 添加终点标记
        addMarker(destinationLocation, destination);
        
        // 绘制模拟路线
        if (map) {
            const path = [BUPT_LOCATION, destinationLocation];
            
            const polyline = new AMap.Polyline({
                path: path,
                isOutline: true,
                outlineColor: '#ffeeee',
                borderWeight: 2,
                strokeWeight: 5,
                strokeColor: '#0091ff',
                lineJoin: 'round'
            });
            
            map.add(polyline);
            map.setFitView();
        }
    } catch (error) {
        console.error('Error drawing route:', error);
    }
}

/**
 * 规划路线（高德API直接调用）
 */
function planRoute(origin, destination, destinationName) {
    // 清除之前的路线
    clearMarkers();
    
    // 添加起点和终点标记
    addMarker(origin, DEFAULT_ORIGIN);
    addMarker(destination, destinationName);
    
    // 调整地图视野以包含所有标记点
    map.setFitView();
    
    // 根据交通方式选择不同的路线规划方法
    switch (transportType) {
        case 'walking':
            planWalkingRoute(origin, destination);
            break;
        case 'driving':
            planDrivingRoute(origin, destination);
            break;
        case 'transit':
            planTransitRoute(origin, destination);
            break;
        case 'bicycling':
            planBicyclingRoute(origin, destination);
            break;
        default:
            planWalkingRoute(origin, destination);
    }
}

/**
 * 步行路线规划
 */
function planWalkingRoute(origin, destination) {
    // 使用Web服务API而不是JavaScript API以提高可靠性
    if (typeof AMap === 'undefined') {
        // 如果AMap未定义，使用直接调用Rest API方式
        useRestApiWalkingRoute(origin, destination);
        return;
    }

    try {
        AMap.plugin('AMap.Walking', function() {
            var walking = new AMap.Walking({
                map: map,
                panel: 'routeSteps',
                key: '9c63c03a728596ab62f12bb8690d536b'  // 使用路线规划API密钥
            });
            
            // 为避免跨域问题，设置回调函数名
            var opts = {
                policy: 0 // 结合实时交通（未知代表最快）
            };
            
            walking.search(origin, destination, opts, function(status, result) {
                hideLoading();
                
                if (status === 'complete') {
                    console.log('步行规划成功:', result);
                    
                    // 提取路线信息
                    if (result.routes && result.routes.length > 0) {
                        const route = {
                            distance: result.routes[0].distance,
                            duration: result.routes[0].time,
                            steps: result.routes[0].steps.map(step => ({
                                instruction: step.instruction,
                                distance: step.distance
                            }))
                        };
                        
                        // 更新路线信息
                        renderRouteInfo(route);
                    }
                } else {
                    console.error('步行路线规划失败:', result);
                    showToast('步行路线规划失败，尝试使用备用方法');
                    // 备用方法：使用Web服务API
                    useRestApiWalkingRoute(origin, destination);
                }
            });
        });
    } catch (error) {
        console.error('步行路线搜索错误:', error);
        showToast('步行路线规划出错，尝试使用备用方法');
        // 备用方法：使用Web服务API
        useRestApiWalkingRoute(origin, destination);
    }
}

/**
 * 使用Rest API进行步行路线规划(备用方法)
 */
function useRestApiWalkingRoute(origin, destination) {
    // 格式化坐标
    const originStr = Array.isArray(origin) ? origin[0] + ',' + origin[1] : origin;
    const destinationStr = Array.isArray(destination) ? destination[0] + ',' + destination[1] : destination;
    
    // 使用jQuery Ajax发起请求
    $.ajax({
        url: '/api/travel/walking-route',
        method: 'GET',
        data: {
            origin: originStr,
            destination: destinationStr
        },
        success: function(response) {
            hideLoading();
            
            // 检查是否有路线结果
            if (response && response.route && response.route.paths && response.route.paths.length > 0) {
                const path = response.route.paths[0];
                
                // 创建路线对象
                const route = {
                    distance: path.distance,
                    duration: path.duration,
                    steps: path.steps && path.steps.map(step => ({
                        instruction: step.instruction || '向前走',
                        distance: step.distance || 0
                    })) || []
                };
                
                // 更新路线信息
                renderRouteInfo(route);
                
                // 模拟在地图上绘制路线
                drawWalkingPathOnMap(path, origin, destination);
            } else {
                console.error('无路线结果数据');
                showToast('获取路线失败，请稍后重试');
            }
        },
        error: function(error) {
            hideLoading();
            console.error('获取步行路线失败:', error);
            showToast('获取路线失败，请稍后重试');
        }
    });
}

/**
 * 在地图上绘制步行路线
 */
function drawWalkingPathOnMap(path, origin, destination) {
    try {
        if (!map) {
            initMap();
        }
        
        // 清除之前的标记
        clearMarkers();
        
        // 添加起点和终点标记
        addMarker(origin, DEFAULT_ORIGIN);
        addMarker(destination, "目的地");
        
        if (path && path.steps) {
            // 收集所有坐标点
            let allPoints = [];
            
            path.steps.forEach(step => {
                if (step.polyline) {
                    // 解析折线坐标
                    const polylinePoints = step.polyline.split(';').map(point => {
                        const coords = point.split(',');
                        return [parseFloat(coords[0]), parseFloat(coords[1])];
                    });
                    
                    allPoints = allPoints.concat(polylinePoints);
                }
            });
            
            // 如果有有效坐标，绘制折线
            if (allPoints.length > 0) {
                const polyline = new AMap.Polyline({
                    path: allPoints,
                    isOutline: true,
                    outlineColor: '#ffeeee',
                    borderWeight: 2,
                    strokeWeight: 5,
                    strokeColor: '#0091ff',
                    lineJoin: 'round'
                });
                
                map.add(polyline);
            }
        }
        
        // 调整地图视野
        map.setFitView();
    } catch (error) {
        console.error('绘制路线错误:', error);
    }
}

/**
 * 驾车路线规划
 */
function planDrivingRoute(origin, destination) {
    // 使用Web服务API而不是JavaScript API以提高可靠性
    if (typeof AMap === 'undefined') {
        // 如果AMap未定义，使用直接调用Rest API方式
        useRestApiDrivingRoute(origin, destination);
        return;
    }
    
    try {
        AMap.plugin('AMap.Driving', function() {
            const driving = new AMap.Driving({
                map: map,
                panel: 'routeSteps',
                key: '9c63c03a728596ab62f12bb8690d536b'  // 使用路线规划API密钥
            });
            
            driving.search(origin, destination, function(status, result) {
                hideLoading();
                
                if (status === 'complete') {
                    // 更新总距离和时间
                    const route = result.routes[0];
                    updateRouteInfoFromAMap(route);
                    
                    // 调整地图以显示完整路线
                    map.setFitView();
                } else {
                    console.error('驾车路线规划失败', status, result);
                    showToast('驾车路线规划失败，尝试使用备用方法');
                    // 备用方法：使用Web服务API
                    useRestApiDrivingRoute(origin, destination);
                }
            });
        });
    } catch (error) {
        console.error('驾车路线搜索错误:', error);
        showToast('驾车路线规划出错，尝试使用备用方法');
        // 备用方法：使用Web服务API
        useRestApiDrivingRoute(origin, destination);
    }
}

/**
 * 使用Rest API进行驾车路线规划(备用方法)
 */
function useRestApiDrivingRoute(origin, destination) {
    // 格式化坐标
    const originStr = Array.isArray(origin) ? origin[0] + ',' + origin[1] : origin;
    const destinationStr = Array.isArray(destination) ? destination[0] + ',' + destination[1] : destination;
    
    // 使用jQuery Ajax发起请求
    $.ajax({
        url: '/api/travel/driving-route',
        method: 'GET',
        data: {
            origin: originStr,
            destination: destinationStr
        },
        success: function(response) {
            hideLoading();
            
            // 检查是否有路线结果
            if (response && response.route && response.route.paths && response.route.paths.length > 0) {
                const path = response.route.paths[0];
                
                // 创建路线对象
                const route = {
                    distance: path.distance,
                    duration: path.duration,
                    steps: path.steps || []
                };
                
                // 更新路线信息
                renderRouteInfo(route);
                
                // 在地图上绘制路线
                drawPathOnMap(path, origin, destination);
            } else {
                console.error('无驾车路线结果数据');
                showToast('获取驾车路线失败，请稍后重试');
            }
        },
        error: function(error) {
            hideLoading();
            console.error('获取驾车路线失败:', error);
            showToast('获取驾车路线失败，请稍后重试');
        }
    });
}

/**
 * 公交路线规划
 */
function planTransitRoute(origin, destination) {
    AMap.plugin('AMap.Transfer', function() {
        const transfer = new AMap.Transfer({
            map: map,
            panel: 'routeSteps',
            city: '北京',
            key: '9c63c03a728596ab62f12bb8690d536b'  // 使用路线规划API密钥
        });
        
        transfer.search(origin, destination, function(status, result) {
            hideLoading();
            
            if (status === 'complete') {
                // 更新总距离和时间
                const route = result.plans[0];
                
                // 公交路线的距离和时间获取方式不同
                $('#totalDistance').text((route.distance / 1000).toFixed(2) + ' 公里');
                $('#totalDuration').text(Math.ceil(route.time / 60) + ' 分钟');
                
                // 调整地图以显示完整路线
                map.setFitView();
            } else {
                showToast('公交路线规划失败，请检查API密钥是否正确');
            }
        });
    });
}

/**
 * 骑行路线规划
 */
function planBicyclingRoute(origin, destination) {
    AMap.plugin('AMap.Riding', function() {
        const riding = new AMap.Riding({
            map: map,
            panel: 'routeSteps',
            key: '9c63c03a728596ab62f12bb8690d536b'  // 使用路线规划API密钥
        });
        
        riding.search(origin, destination, function(status, result) {
            hideLoading();
            
            if (status === 'complete') {
                // 更新总距离和时间
                const route = result.routes[0];
                updateRouteInfoFromAMap(route);
                
                // 调整地图以显示完整路线
                map.setFitView();
            } else {
                showToast('骑行路线规划失败，请检查API密钥是否正确');
            }
        });
    });
}

/**
 * 更新路线信息（从高德API结果）
 */
function updateRouteInfoFromAMap(route) {
    // 更新总距离
    $('#totalDistance').text((route.distance / 1000).toFixed(2) + ' 公里');
    
    // 更新总时间
    $('#totalDuration').text(Math.ceil(route.time / 60) + ' 分钟');
}

/**
 * 渲染路线信息
 */
function renderRouteInfo(route) {
    try {
        // 如果route是字符串，尝试解析
        if (typeof route === 'string') {
            try {
                route = JSON.parse(route);
            } catch (e) {
                console.error('JSON解析错误:', e);
                // 如果无法解析，设置默认值
                route = { distance: 0, duration: 0, steps: [] };
            }
        }
        
        // 确保route是对象
        if (!route || typeof route !== 'object') {
            console.error('路线数据格式错误:', route);
            route = { distance: 0, duration: 0, steps: [] };
        }
        
        // 显示总距离
        let distanceText = '';
        if (route.distance > 1000) {
            distanceText = (route.distance / 1000).toFixed(1) + ' 公里';
        } else {
            distanceText = route.distance + ' 米';
        }
        $('#totalDistance').text(distanceText);
        
        // 显示总时间
        let durationText = '';
        if (route.duration > 3600) {
            const hours = Math.floor(route.duration / 3600);
            const minutes = Math.floor((route.duration % 3600) / 60);
            durationText = hours + ' 小时 ' + minutes + ' 分钟';
        } else if (route.duration > 60) {
            durationText = Math.floor(route.duration / 60) + ' 分钟';
        } else {
            durationText = route.duration + ' 秒';
        }
        $('#totalDuration').text(durationText);
        
        // 清空步骤容器
        $('#routeSteps').empty();
        
        // 如果步骤信息在steps字段中，显示步骤
        if (route.steps && route.steps.length > 0) {
            console.log('显示路线步骤，共' + route.steps.length + '步');
        }
    } catch (error) {
        console.error('渲染路线信息错误:', error);
    }
}

/**
 * 显示加载指示器
 */
function showLoading() {
    // 可以在这里添加加载动画
    $('#routeSteps').html('<div class="text-center p-3"><div class="spinner-border text-primary" role="status"></div><div class="mt-2">正在规划路线...</div></div>');
}

/**
 * 隐藏加载指示器
 */
function hideLoading() {
    // 隐藏加载动画
}

/**
 * 显示提示消息
 */
function showToast(message) {
    // 创建一个Bootstrap toast
    const toastId = 'system-toast-' + Date.now();
    const toast = `
    <div class="position-fixed bottom-0 end-0 p-3" style="z-index: 5">
        <div id="${toastId}" class="toast" role="alert" aria-live="assertive" aria-atomic="true">
            <div class="toast-header">
                <strong class="me-auto">消息提示</strong>
                <button type="button" class="btn-close" data-bs-dismiss="toast" aria-label="Close"></button>
            </div>
            <div class="toast-body">
                ${message}
            </div>
        </div>
    </div>`;
    
    // 添加到DOM
    $(document.body).append(toast);
    
    // 显示toast
    const toastElement = new bootstrap.Toast(document.getElementById(toastId));
    toastElement.show();
    
    // 一定时间后移除
    setTimeout(() => {
        $(`#${toastId}`).parent().remove();
    }, 5000);
}

/**
 * 添加预定义目的地标记
 */
function addPopularDestinationMarkers() {
    // 创建不同颜色的标记
    const colors = ['blue', 'red', 'green', 'purple', 'orange'];
    let colorIndex = 0;
    
    for (const [name, position] of Object.entries(POPULAR_DESTINATIONS)) {
        // 跳过一些标记，以避免地图过于拥挤
        if (Math.random() < 0.6) continue;
        
        try {
            // 创建标记
            const marker = new AMap.Marker({
                position: position,
                title: name,
                map: map,
                animation: 'AMAP_ANIMATION_DROP'
            });
            
            // 添加信息窗体
            const infoWindow = new AMap.InfoWindow({
                content: `<div class="info-window"><h5>${name}</h5><button class="btn btn-sm btn-primary route-here-btn">规划路线</button></div>`,
                offset: new AMap.Pixel(0, -30)
            });
            
            // 点击标记时显示信息窗体
            marker.on('click', function() {
                infoWindow.open(map, marker.getPosition());
                
                // 使用setTimeout确保DOM元素已经渲染
                setTimeout(() => {
                    const routeBtn = document.querySelector('.route-here-btn');
                    if (routeBtn) {
                        routeBtn.addEventListener('click', function() {
                            $('#destination').val(name);
                            searchRoute();
                            infoWindow.close();
                        });
                    }
                }, 100);
            });
            
            colorIndex = (colorIndex + 1) % colors.length;
        } catch (error) {
            console.error('Error adding marker for ' + name, error);
        }
    }
}

/**
 * 在地图上绘制通用路径
 */
function drawPathOnMap(path, origin, destination) {
    try {
        if (!map) {
            initMap();
        }
        
        // 清除之前的标记
        clearMarkers();
        
        // 添加起点和终点标记
        addMarker(origin, DEFAULT_ORIGIN);
        addMarker(destination, "目的地");
        
        // 折线点集合
        let polylinePaths = [];
        
        if (path && path.steps && path.steps.length > 0) {
            console.log('解析路径步骤，共', path.steps.length, '步');
            
            // 遍历每个步骤
            path.steps.forEach(step => {
                if (step.polyline) {
                    console.log('处理折线：', step.polyline.substring(0, 30) + '...');
                    
                    // 解析折线坐标
                    const polylinePoints = step.polyline.split(';').map(pointStr => {
                        const coords = pointStr.split(',');
                        if (coords.length >= 2) {
                            return [parseFloat(coords[0]), parseFloat(coords[1])];
                        }
                        return null;
                    }).filter(point => point !== null);
                    
                    if (polylinePoints.length > 0) {
                        polylinePaths = polylinePaths.concat(polylinePoints);
                    }
                }
            });
            
            console.log('收集到', polylinePaths.length, '个路径点');
            
            // 绘制路线
            if (polylinePaths.length > 0) {
                const polyline = new AMap.Polyline({
                    path: polylinePaths,
                    isOutline: true,
                    outlineColor: '#ffeeee',
                    borderWeight: 2,
                    strokeWeight: 5,
                    strokeColor: '#0091ff',
                    lineJoin: 'round'
                });
                
                map.add(polyline);
                console.log('路线已添加到地图');
                
                // 调整地图视野
                map.setFitView();
                return true;
            } else {
                console.warn('未收集到任何路径点，回退到简单直线');
                drawSimplePath(origin, destination);
                return false;
            }
        } else {
            console.warn('路径数据无效或没有步骤信息，回退到简单直线');
            drawSimplePath(origin, destination);
            return false;
        }
    } catch (error) {
        console.error('绘制路线错误:', error);
        drawSimplePath(origin, destination);
        return false;
    }
} 