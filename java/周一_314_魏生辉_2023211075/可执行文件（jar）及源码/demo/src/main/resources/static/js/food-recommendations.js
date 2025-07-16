$(document).ready(function() {
    console.log("Food recommendations page loaded");
    
    // Variables to track current state
    let currentFoodList = [];
    let currentFilters = {
        cuisine: [],
        minRating: 0,
        sortBy: 'createdAt', // Default sort by newest
        sortOrder: 'desc'
    };
    let selectedFoodId = null;

    // Initialize the page
    loadAllFoodRecommendations();

    // Event handlers
    $('#searchButton').click(function() {
        console.log("Search button clicked");
        const searchText = $('#searchInput').val().trim();
        if (searchText) {
            searchFoodRecommendations(searchText);
        } else {
            loadAllFoodRecommendations();
        }
    });

    $('#searchInput').on('keypress', function(e) {
        if (e.which === 13) { // Enter key
            $('#searchButton').click();
        }
    });

    $('.cuisine-filter').on('change', function() {
        updateCuisineFilters();
    });

    $('#ratingFilter').on('change', function() {
        currentFilters.minRating = parseFloat($(this).val());
    });

    $('#sortByFilter').on('change', function() {
        currentFilters.sortBy = $(this).val();
        applySorting();
    });

    $('#sortOrderFilter').on('change', function() {
        currentFilters.sortOrder = $(this).val();
        applySorting();
    });

    $('#applyFiltersBtn').click(function() {
        applyFilters();
    });

    $('#saveFoodBtn').click(function() {
        saveFoodRecommendation();
    });

    $('#foodRecommendationsList').on('click', '.food-card', function() {
        const foodId = $(this).data('id');
        openFoodDetails(foodId);
    });

    $('#editFoodBtn').click(function() {
        openEditFoodModal();
    });

    $('#updateFoodBtn').click(function() {
        updateFoodRecommendation();
    });

    $('#deleteFoodBtn').click(function() {
        confirmDeleteFood();
    });

    // Functions for API calls and data handling
    function loadAllFoodRecommendations() {
        console.log("Loading all food recommendations...");
        $.ajax({
            url: '/api/food-recommendations',
            type: 'GET',
            success: function(data) {
                console.log("API response received:", data);
                console.log("Number of food items:", data.length);
                currentFoodList = data;
                renderFoodRecommendations(data);
                updatePopularityBadges(data);
            },
            error: function(error) {
                console.error('Error loading food recommendations:', error);
                showAlert('danger', '加载美食推荐失败，请稍后再试');
            }
        });
    }

    function searchFoodRecommendations(searchText) {
        $.ajax({
            url: `/api/food-recommendations/search?name=${encodeURIComponent(searchText)}`,
            type: 'GET',
            success: function(data) {
                currentFoodList = data;
                renderFoodRecommendations(data);
                updatePopularityBadges(data);
            },
            error: function(error) {
                console.error('Error searching food recommendations:', error);
                showAlert('danger', '搜索失败，请稍后再试');
            }
        });
    }

    function updateCuisineFilters() {
        const selectedCuisines = [];
        $('.cuisine-filter:checked').each(function() {
            selectedCuisines.push($(this).val());
        });
        currentFilters.cuisine = selectedCuisines;
    }

    function applyFilters() {
        let filteredList = [...currentFoodList];

        // Apply cuisine filter
        if (currentFilters.cuisine.length > 0) {
            filteredList = filteredList.filter(food => 
                currentFilters.cuisine.includes(food.cuisine));
        }

        // Apply rating filter
        if (currentFilters.minRating > 0) {
            filteredList = filteredList.filter(food => 
                food.rating >= currentFilters.minRating);
        }

        // Apply sorting
        applySortingToList(filteredList);

        renderFoodRecommendations(filteredList);
    }

    function applySorting() {
        applySortingToList(currentFoodList);
        renderFoodRecommendations(currentFoodList);
    }

    function applySortingToList(list) {
        list.sort((a, b) => {
            let comparison = 0;
            
            switch (currentFilters.sortBy) {
                case 'rating':
                    comparison = a.rating - b.rating;
                    break;
                case 'popularity':
                    // Assuming we have a popularity field - could be view count
                    comparison = (a.viewCount || 0) - (b.viewCount || 0);
                    break;
                case 'name':
                    comparison = a.name.localeCompare(b.name);
                    break;
                default: // createdAt
                    comparison = new Date(a.createdAt) - new Date(b.createdAt);
            }
            
            return currentFilters.sortOrder === 'asc' ? comparison : -comparison;
        });
    }

    function updatePopularityBadges(foodList) {
        // Sort by rating to find top rated
        const topRated = [...foodList].sort((a, b) => b.rating - a.rating).slice(0, 3);
        
        // Mark top rated foods
        topRated.forEach(food => {
            $(`.food-card[data-id="${food.id}"] .popularity-badge`).html(
                '<span class="badge bg-danger me-1"><i class="fas fa-fire"></i> 最高评分</span>'
            );
        });
    }

    function renderFoodRecommendations(foodList) {
        console.log("Rendering food recommendations, count:", foodList.length);
        const $container = $('#foodRecommendationsList');
        $container.empty();

        if (foodList.length === 0) {
            console.log("No food items to display");
            $container.html('<div class="col-12 text-center"><p>没有找到符合条件的美食推荐</p></div>');
            return;
        }

        // 添加一条测试数据，检查渲染是否正常
        if (foodList.length > 0) {
            console.log("Sample food item:", foodList[0]);
        }

        foodList.forEach((food, index) => {
            const cuisineLabel = getCuisineLabel(food.cuisine);
            const ratingStars = generateRatingStars(food.rating);
            
            const cardHtml = `
                <div class="col-md-4 mb-4">
                    <div class="card food-card h-100" data-id="${food.id}" data-cuisine="${food.cuisine}">
                        <div class="card-body">
                            <div class="popularity-badge mb-2"></div>
                            <h5 class="card-title">${food.name}</h5>
                            <div class="mb-2">
                                <span class="badge bg-primary">${cuisineLabel}</span>
                            </div>
                            <div class="mb-2">
                                ${ratingStars}
                            </div>
                            <p class="card-text text-truncate"><i class="fas fa-map-marker-alt text-danger me-2"></i>${food.location}</p>
                            <p class="card-text description-text">${food.description}</p>
                        </div>
                        <div class="card-footer">
                            <button class="btn btn-sm btn-outline-primary view-details-btn">查看详情</button>
                        </div>
                    </div>
                </div>
            `;
            $container.append(cardHtml);
            console.log(`Rendered food item ${index + 1}: ${food.name}`);
        });
    }

    function getCuisineLabel(cuisine) {
        const labels = {
            'chinese': '中餐',
            'japanese': '日餐',
            'korean': '韩餐',
            'western': '西餐',
            'other': '其他'
        };
        return labels[cuisine] || cuisine;
    }

    function generateRatingStars(rating) {
        let starsHtml = `<div class="rating-stars">`;
        const fullStars = Math.floor(rating);
        const halfStar = rating % 1 >= 0.5;
        
        for (let i = 1; i <= 5; i++) {
            if (i <= fullStars) {
                starsHtml += `<i class="fas fa-star"></i>`;
            } else if (i === fullStars + 1 && halfStar) {
                starsHtml += `<i class="fas fa-star-half-alt"></i>`;
            } else {
                starsHtml += `<i class="far fa-star"></i>`;
            }
        }
        
        starsHtml += ` <span class="rating-value">${rating.toFixed(1)}</span></div>`;
        return starsHtml;
    }

    function openFoodDetails(foodId) {
        const food = currentFoodList.find(item => item.id === foodId);
        if (!food) return;
        
        selectedFoodId = foodId;
        
        // Update view counter (if backend supports this)
        incrementViewCount(foodId);
        
        // Populate modal
        $('#viewFoodName').text(food.name);
        $('#viewFoodDescription').html(food.description);
        $('#viewFoodCuisine').text(getCuisineLabel(food.cuisine));
        $('#viewFoodLocation').text(food.location);
        $('#viewFoodRating').html(generateRatingStars(food.rating));
        $('#viewFoodImage').hide(); // 隐藏图片元素
        
        // Show modal
        $('#viewFoodModal').modal('show');
    }

    function incrementViewCount(foodId) {
        // This would require a backend endpoint - placeholder for now
        $.ajax({
            url: `/api/food-recommendations/${foodId}/view`,
            type: 'POST',
            error: function(error) {
                console.error('Error updating view count:', error);
            }
        });
    }

    function openEditFoodModal() {
        if (!selectedFoodId) return;
        
        const food = currentFoodList.find(item => item.id === selectedFoodId);
        if (!food) return;
        
        // Hide view modal
        $('#viewFoodModal').modal('hide');
        
        // Populate edit form
        $('#editFoodId').val(food.id);
        $('#editFoodName').val(food.name);
        $('#editFoodDescription').val(food.description);
        $('#editFoodCuisine').val(food.cuisine);
        $('#editFoodLocation').val(food.location);
        $('#editFoodRating').val(food.rating);
        $('#editFoodImageUrl').val(food.imageUrl);
        
        // Show edit modal
        $('#editFoodModal').modal('show');
    }

    function saveFoodRecommendation() {
        const foodData = {
            name: $('#foodName').val(),
            description: $('#foodDescription').val(),
            cuisine: $('#foodCuisine').val(),
            location: $('#foodLocation').val(),
            rating: parseFloat($('#foodRating').val()),
            imageUrl: $('#foodImageUrl').val() || null
        };
        
        $.ajax({
            url: '/api/food-recommendations',
            type: 'POST',
            contentType: 'application/json',
            data: JSON.stringify(foodData),
            success: function(data) {
                $('#createFoodModal').modal('hide');
                $('#createFoodForm')[0].reset();
                showAlert('success', '美食推荐添加成功！');
                loadAllFoodRecommendations();
            },
            error: function(error) {
                console.error('Error creating food recommendation:', error);
                showAlert('danger', '添加失败，请稍后再试');
            }
        });
    }

    function updateFoodRecommendation() {
        const foodId = $('#editFoodId').val();
        if (!foodId) return;
        
        const foodData = {
            name: $('#editFoodName').val(),
            description: $('#editFoodDescription').val(),
            cuisine: $('#editFoodCuisine').val(),
            location: $('#editFoodLocation').val(),
            rating: parseFloat($('#editFoodRating').val()),
            imageUrl: $('#editFoodImageUrl').val() || null
        };
        
        $.ajax({
            url: `/api/food-recommendations/${foodId}`,
            type: 'PUT',
            contentType: 'application/json',
            data: JSON.stringify(foodData),
            success: function(data) {
                $('#editFoodModal').modal('hide');
                showAlert('success', '美食推荐更新成功！');
                loadAllFoodRecommendations();
            },
            error: function(error) {
                console.error('Error updating food recommendation:', error);
                showAlert('danger', '更新失败，请稍后再试');
            }
        });
    }

    function confirmDeleteFood() {
        if (confirm('确定要删除这条美食推荐吗？此操作不可撤销！')) {
            deleteFoodRecommendation();
        }
    }

    function deleteFoodRecommendation() {
        if (!selectedFoodId) return;
        
        $.ajax({
            url: `/api/food-recommendations/${selectedFoodId}`,
            type: 'DELETE',
            success: function() {
                $('#viewFoodModal').modal('hide');
                showAlert('success', '美食推荐已删除');
                loadAllFoodRecommendations();
            },
            error: function(error) {
                console.error('Error deleting food recommendation:', error);
                showAlert('danger', '删除失败，请稍后再试');
            }
        });
    }

    function showAlert(type, message) {
        const alertHtml = `
            <div class="alert alert-${type} alert-dismissible fade show" role="alert">
                ${message}
                <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
            </div>
        `;
        
        $('#alertContainer').html(alertHtml);
        setTimeout(() => {
            $('.alert').alert('close');
        }, 5000);
    }

    // 在页面底部添加一个简单的状态显示
    $('body').append('<div id="debug-info" style="position:fixed; bottom:10px; right:10px; background:rgba(0,0,0,0.7); color:white; padding:10px; border-radius:5px;">加载中...</div>');
    
    // 加载完成后更新状态
    setTimeout(function() {
        $('#debug-info').text(`加载了 ${currentFoodList.length} 条美食数据`);
    }, 3000);
}); 