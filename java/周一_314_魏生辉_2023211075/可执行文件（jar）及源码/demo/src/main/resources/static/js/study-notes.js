$(document).ready(function() {
    // Page view management
    const $studyNotesView = $('#studyNotesView');
    const $studyParadiseView = $('#studyParadiseView');
    const $wordLearningView = $('#wordLearningView');
    const $navStudyNotes = $('#navStudyNotes');
    const $navStudyParadise = $('#navStudyParadise');

    // Initially show Study Paradise, hide others
    $studyNotesView.hide();
    $wordLearningView.hide();
    $studyParadiseView.show();

    // Navigation clicks
    $navStudyNotes.click(function(e) {
        e.preventDefault();
        $navStudyParadise.removeClass('active');
        $(this).addClass('active');
        $studyParadiseView.hide();
        $wordLearningView.hide();
        $studyNotesView.show();
        loadNotes(); // Load notes when switching to this view
    });

    $navStudyParadise.click(function(e) {
        e.preventDefault();
        $navStudyNotes.removeClass('active');
        $(this).addClass('active');
        $studyNotesView.hide();
        $wordLearningView.hide();
        $studyParadiseView.show();
    });

    // --- Study Paradise Page Logic ---
    $('#btnTodayStudy').click(function() {
        $studyParadiseView.hide();
        $wordLearningView.show();
        loadDailyWords();
    });

    $('#btnFunGame').click(function() {
        window.location.href = '/study/cet6-game';
    });

    $('#btnOverallProgress').click(function() {
        window.location.href = '/study/study-progress';
    });

    // New button for '记笔记'
    $('#btnTakeNotes').click(function(e) {
        e.preventDefault();
        $navStudyParadise.removeClass('active'); // Deactivate Study Paradise nav item
        $navStudyNotes.addClass('active');     // Activate Study Notes nav item
        $studyParadiseView.hide();
        $wordLearningView.hide();
        $studyNotesView.show();
        loadNotes(); // Load notes when switching to this view
    });

    // --- Word Learning Page Logic ---
    let dailyWords = [];
    let currentWordIndex = 0;
    let isPracticeMode = false;

    $('#backToParadise').click(function(e) {
        e.preventDefault();
        $wordLearningView.hide();
        $studyParadiseView.show();
        $('#wordCard').removeClass('flipped'); // Reset card flip
        isPracticeMode = false; // Reset mode
        $('#btnStudyMode').addClass('btn-primary').removeClass('btn-outline-primary');
        $('#btnPracticeMode').removeClass('btn-primary').addClass('btn-outline-primary');
        $('#practiceOptions').hide();
        $('#practiceFeedback').empty();
    });

    $('#wordCard').click(function() {
        if (!isPracticeMode) {
            $(this).toggleClass('flipped');
        }
    });

    $('#btnPreviousWord').click(function() {
        if (currentWordIndex > 0) {
            currentWordIndex--;
            updateWordCard();
        }
    });

    $('#btnNextWord').click(function() {
        if (currentWordIndex < dailyWords.length - 1) {
            currentWordIndex++;
            updateWordCard();
        } else {
            alert('恭喜你，今天的单词都学完了！');
            // Optionally, go back to study paradise or show a summary
            $('#backToParadise').click();
        }
    });

    $('#btnCompleteStudy').click(function() {
        alert('你已完成今天的学习！');
        $('#backToParadise').click();
    });

    $('#btnStudyMode').click(function() {
        isPracticeMode = false;
        $(this).addClass('btn-primary').removeClass('btn-outline-primary');
        $('#btnPracticeMode').removeClass('btn-primary').addClass('btn-outline-primary');
        $('#wordCard').removeClass('flipped'); // Ensure card is not flipped in study mode
        $('#practiceOptions').hide();
        $('#practiceFeedback').empty();
        updateWordCard(); // Redraw card for study mode
    });

    $('#btnPracticeMode').click(function() {
        isPracticeMode = true;
        $(this).addClass('btn-primary').removeClass('btn-outline-primary');
        $('#btnStudyMode').removeClass('btn-primary').addClass('btn-outline-primary');
        $('#wordCard').removeClass('flipped'); // Ensure card is not flipped in practice mode
        $('#practiceOptions').show();
        $('#practiceFeedback').empty();
        generatePracticeQuestion();
    });

    function loadDailyWords() {
        // Fetch 10 daily words from the backend
        $.ajax({
            url: '/api/cet6words/daily/10',
            type: 'GET',
            success: function(words) {
                if (words && words.length > 0) {
                    dailyWords = words;
                    currentWordIndex = 0;
                    updateWordCard();
                    // Set current day and category (dummy for now)
                    $('#currentDay').text('6'); // Example: Day 6
                    $('.text-muted span:contains("(Family)")').text('(CET-6)'); // Example: Category
                } else {
                    dailyWords = [ // Dummy data if API fails or returns empty
                        { id: 1, word: 'ubiquitous', definition: '普遍存在的，无处不在的', pronunciation: '', category: 'General', difficulty: 'advanced' },
                        { id: 2, word: 'meticulous', definition: '一丝不苟的，非常仔细的', pronunciation: '', category: 'General', difficulty: 'advanced' },
                        { id: 3, word: 'benevolent', definition: '仁慈的，善良的', pronunciation: '', category: 'General', difficulty: 'intermediate' },
                        { id: 4, word: 'ephemeral', definition: '短暂的，瞬息的', pronunciation: '', category: 'Abstract', difficulty: 'advanced' },
                        { id: 5, word: 'gregarious', definition: '好交际的，合群的', pronunciation: '', category: 'Personality', difficulty: 'intermediate' },
                        { id: 6, word: 'paradox', definition: '悖论，反论', pronunciation: '', category: 'Abstract', difficulty: 'advanced' },
                        { id: 7, word: 'serendipity', definition: '意外发现的才能，机缘巧合', pronunciation: '', category: 'Abstract', difficulty: 'advanced' },
                        { id: 8, word: 'cacophony', definition: '刺耳的嘈杂声，不和谐音', pronunciation: '', category: 'Sound', difficulty: 'advanced' },
                        { id: 9, word: 'equivocal', definition: '模棱两可的，含糊不清的', pronunciation: '', category: 'Communication', difficulty: 'advanced' },
                        { id: 10, word: 'plethora', definition: '过多，过剩', pronunciation: '', category: 'Quantity', difficulty: 'intermediate' },
                        { id: 11, word: 'ostentatious', definition: '炫耀的，卖弄的', pronunciation: '', category: 'Personality', difficulty: 'advanced' },
                        { id: 12, word: 'perfunctory', definition: '敷衍的，马虎的', pronunciation: '', category: 'Action', difficulty: 'advanced' },
                        { id: 13, word: 'capricious', definition: '反复无常的，任性的', pronunciation: '', category: 'Personality', difficulty: 'intermediate' },
                        { id: 14, word: 'dogmatic', definition: '教条的，武断的', pronunciation: '', category: 'Opinion', difficulty: 'advanced' },
                        { id: 15, word: 'reclusive', definition: '隐居的，孤寂的', pronunciation: '', category: 'Personality', difficulty: 'intermediate' },
                        { id: 16, word: 'superfluous', definition: '多余的，不必要的', pronunciation: '', category: 'General', difficulty: 'advanced' },
                        { id: 17, word: 'alacrity', definition: '敏捷，欣然', pronunciation: '', category: 'Emotion', difficulty: 'advanced' },
                        { id: 18, word: 'platitude', definition: '陈词滥调', pronunciation: '', category: 'Communication', difficulty: 'intermediate' },
                        { id: 19, word: 'innocuous', definition: '无害的', pronunciation: '', category: 'General', difficulty: 'intermediate' },
                        { id: 20, word: 'sycophant', definition: '马屁精', pronunciation: '', category: 'Personality', difficulty: 'advanced' }
                    ];
                    currentWordIndex = 0;
                    updateWordCard();
                    $('#currentDay').text('6');
                    $('.text-muted span:contains("(Family)")').text('(CET-6)');
                    console.warn('API /api/cet6words/daily/10 returned no words or failed. Using dummy data.');
                }
            },
            error: function(xhr, status, error) {
                console.error('Error fetching daily words:', xhr, status, error);
                dailyWords = [ // Dummy data on error
                    { id: 1, word: 'ubiquitous', definition: '普遍存在的，无处不在的', pronunciation: '', category: 'General', difficulty: 'advanced' },
                    { id: 2, word: 'meticulous', definition: '一丝不苟的，非常仔细的', pronunciation: '', category: 'General', difficulty: 'advanced' },
                    { id: 3, word: 'benevolent', definition: '仁慈的，善良的', pronunciation: '', category: 'General', difficulty: 'intermediate' },
                    { id: 4, word: 'ephemeral', definition: '短暂的，瞬息的', pronunciation: '', category: 'Abstract', difficulty: 'advanced' },
                    { id: 5, word: 'gregarious', definition: '好交际的，合群的', pronunciation: '', category: 'Personality', difficulty: 'intermediate' },
                    { id: 6, word: 'paradox', definition: '悖论，反论', pronunciation: '', category: 'Abstract', difficulty: 'advanced' },
                    { id: 7, word: 'serendipity', definition: '意外发现的才能，机缘巧合', pronunciation: '', category: 'Abstract', difficulty: 'advanced' },
                    { id: 8, word: 'cacophony', definition: '刺耳的嘈杂声，不和谐音', pronunciation: '', category: 'Sound', difficulty: 'advanced' },
                    { id: 9, word: 'equivocal', definition: '模棱两可的，含糊不清的', pronunciation: '', category: 'Communication', difficulty: 'advanced' },
                    { id: 10, word: 'plethora', definition: '过多，过剩', pronunciation: '', category: 'Quantity', difficulty: 'intermediate' },
                    { id: 11, word: 'ostentatious', definition: '炫耀的，卖弄的', pronunciation: '', category: 'Personality', difficulty: 'advanced' },
                    { id: 12, word: 'perfunctory', definition: '敷衍的，马虎的', pronunciation: '', category: 'Action', difficulty: 'advanced' },
                    { id: 13, word: 'capricious', definition: '反复无常的，任性的', pronunciation: '', category: 'Personality', difficulty: 'intermediate' },
                    { id: 14, word: 'dogmatic', definition: '教条的，武断的', pronunciation: '', category: 'Opinion', difficulty: 'advanced' },
                    { id: 15, word: 'reclusive', definition: '隐居的，孤寂的', pronunciation: '', category: 'Personality', difficulty: 'intermediate' },
                    { id: 16, word: 'superfluous', definition: '多余的，不必要的', pronunciation: '', category: 'General', difficulty: 'advanced' },
                    { id: 17, word: 'alacrity', definition: '敏捷，欣然', pronunciation: '', category: 'Emotion', difficulty: 'advanced' },
                    { id: 18, word: 'platitude', definition: '陈词滥调', pronunciation: '', category: 'Communication', difficulty: 'intermediate' },
                    { id: 19, word: 'innocuous', definition: '无害的', pronunciation: '', category: 'General', difficulty: 'intermediate' },
                    { id: 20, word: 'sycophant', definition: '马屁精', pronunciation: '', category: 'Personality', difficulty: 'advanced' }
                ];
                currentWordIndex = 0;
                updateWordCard();
                $('#currentDay').text('6');
                $('.text-muted span:contains("(Family)")').text('(CET-6)');
            }
        });
    }

    function updateWordCard() {
        if (dailyWords.length === 0) return;

        const currentWord = dailyWords[currentWordIndex];

        $('#cardWord').text(currentWord.word);
        $('#cardDefinition').text(currentWord.definition);
        $('#cardCategory').text(currentWord.category);
        $('#cardDifficulty').text(currentWord.difficulty);
        $('#wordCard').removeClass('flipped'); // Reset flip state

        // Update progress bar
        const progress = ((currentWordIndex + 1) / dailyWords.length) * 100;
        $('#wordProgressBar').css('width', progress + '%').attr('aria-valuenow', progress);
        $('#wordProgressCount').text(`${currentWordIndex + 1}/${dailyWords.length}`);

        // Handle practice mode display
        if (isPracticeMode) {
            $('#cardBack').hide(); // Hide back face in practice mode
            generatePracticeQuestion();
        } else {
            $('#cardBack').show(); // Show back face in study mode
            $('#practiceOptions').hide();
            $('#practiceFeedback').empty();
        }
    }

    function generatePracticeQuestion() {
        if (dailyWords.length === 0) return;

        const currentWord = dailyWords[currentWordIndex];
        const correctAnswer = currentWord.definition;
        const options = [correctAnswer];

        // Get 3 random incorrect options
        while (options.length < 4) {
            const randomIndex = Math.floor(Math.random() * dailyWords.length);
            const randomDefinition = dailyWords[randomIndex].definition;
            if (!options.includes(randomDefinition)) {
                options.push(randomDefinition);
            }
        }

        // Shuffle options
        options.sort(() => Math.random() - 0.5);

        $('#practiceQuestion').text('请选择 ' + currentWord.word + ' 的正确释义:');
        const $optionButtons = $('.practice-option-btn');
        $optionButtons.removeClass('btn-success btn-danger').prop('disabled', false);
        $('#practiceFeedback').empty();

        $optionButtons.each(function(index) {
            $(this).text(options[index]).off('click').click(function() {
                handlePracticeAnswer($(this), options[index] === correctAnswer);
            });
        });
    }

    function handlePracticeAnswer($clickedButton, isCorrect) {
        $('.practice-option-btn').prop('disabled', true); // Disable all options after selection
        if (isCorrect) {
            $clickedButton.addClass('btn-success');
            $('#practiceFeedback').text('回答正确！').css('color', 'green');
        } else {
            $clickedButton.addClass('btn-danger');
            $('#practiceFeedback').text('回答错误。正确答案是: ' + dailyWords[currentWordIndex].definition).css('color', 'red');
            // Highlight the correct answer
            $('.practice-option-btn').each(function() {
                if ($(this).text() === dailyWords[currentWordIndex].definition) {
                    $(this).addClass('btn-success');
                }
            });
        }
        // Automatically move to the next word after a short delay
        setTimeout(() => {
            $('#btnNextWord').click();
        }, 1500);
    }

    // --- Existing Study Notes Features (retained) ---

    // Load all notes on page load
    // This will be called only when the '学习笔记' tab is active
    // loadNotes(); // Removed from initial load, now called by tab click
    
    // Category filter click handler
    $('.category-item').click(function() {
        $('.category-item').removeClass('active');
        $(this).addClass('active');
        
        const category = $(this).data('category');
        if (category === 'all') {
            loadNotes();
        } else {
            loadNotesByCategory(category);
        }
    });
    
    // Search button click handler
    $('#searchButton').click(function() {
        const searchTerm = $('#searchInput').val().trim();
        if (searchTerm) {
            searchNotes(searchTerm);
        } else {
            loadNotes();
        }
    });
    
    // Enter key in search input
    $('#searchInput').keypress(function(e) {
        if (e.which === 13) {
            $('#searchButton').click();
            return false;
        }
    });
    
    // Save note button click handler
    $('#saveNoteBtn').click(function() {
        const noteId = $(this).data('id');
        const isEdit = !!noteId;
        
        const note = {
            title: $('#noteTitle').val(),
            category: $('#noteCategory').val(),
            content: $('#noteContent').val()
        };
        
        if (isEdit) {
            updateNote(noteId, note);
        } else {
            createNote(note);
        }
    });
    
    // Edit note button click handler
    $(document).on('click', '#editNoteBtn', function() {
        const noteId = $(this).data('id');
        
        // Fill the edit form with note data
        $('#noteTitle').val($('#viewNoteTitle').text());
        $('#noteCategory').val($('#viewNoteCategory').text().toLowerCase());
        $('#noteContent').val($('#viewNoteContent').html());
        
        // Set the noteId to the save button
        $('#saveNoteBtn').data('id', noteId);
        
        // Hide view modal and show edit modal
        $('#viewNoteModal').modal('hide');
        $('#createNoteModal').modal('show');
    });
    
    // Delete note button click handler
    $(document).on('click', '#deleteNoteBtn', function() {
        const noteId = $(this).data('id');
        
        if (confirm('确定要删除这条笔记吗？')) {
            deleteNote(noteId);
        }
    });
    
    // Note card click handler for viewing
    $(document).on('click', '.note-card', function() {
        const noteId = $(this).data('id');
        viewNote(noteId);
    });
    
    // Reset form when modal is closed
    $('#createNoteModal').on('hidden.bs.modal', function() {
        $('#createNoteForm')[0].reset();
        $('#saveNoteBtn').removeData('id');
    });

    // 邮学家聊天功能
    $('#sendMessage').click(function() {
        sendMessage();
    });

    $('#userInput').keypress(function(e) {
        if (e.which == 13) {
            sendMessage();
        }
    });

    function showLoading() {
        const loadingHtml = `
            <div class="message bot-message loading-container" id="loadingMessage">
                <div class="loading-dots">
                    <span></span>
                    <span></span>
                    <span></span>
                </div>
                <div>邮学家正在思考中...</div>
            </div>
        `;
        $('#chatContainer').append(loadingHtml);
        $('#loadingMessage').show();
        $('#chatContainer').scrollTop($('#chatContainer')[0].scrollHeight);
    }

    function hideLoading() {
        $('#loadingMessage').remove();
    }

    function sendMessage() {
        const userInput = $('#userInput').val().trim();
        if (userInput === '') return;

        // Add user message to chat
        addMessage(userInput, 'user');
        $('#userInput').val('');

        // Show loading animation
        showLoading();

        // Send to backend
        $.ajax({
            url: '/api/study-assistant/ask',
            method: 'POST',
            contentType: 'application/json',
            data: JSON.stringify({ question: userInput }),
            success: function(response) {
                hideLoading();
                addMessage(response.response, 'bot');
            },
            error: function() {
                hideLoading();
                addMessage('抱歉，我暂时无法回答这个问题。请稍后再试。', 'bot');
            }
        });
    }

    function addMessage(message, type) {
        const messageDiv = $('<div>').addClass('message ' + type + '-message');
        const contentDiv = $('<div>').addClass('message-content').text(message);
        messageDiv.append(contentDiv);
        $('#chatContainer').append(messageDiv);
        $('#chatContainer').scrollTop($('#chatContainer')[0].scrollHeight);
    }
});

// Load all notes
function loadNotes() {
    // In a real application, you would fetch data from the server
    // For this example, we'll use dummy data
    $.ajax({
        url: '/api/study-notes',
        type: 'GET',
        success: function(data) {
            renderNotes(data);
        },
        error: function(xhr) {
            console.error('Error loading notes:', xhr);
            
            // For demonstration, create some dummy data if the API is not available
            const dummyNotes = [
                {
                    id: 1,
                    title: "JavaScript 基础知识",
                    category: "programming",
                    content: "JavaScript 是一种具有函数优先特性的轻量级解释型或者JIT编译型的编程语言。",
                    createdAt: "2023-04-15T10:30:00"
                },
                {
                    id: 2,
                    title: "英语常用词汇",
                    category: "language",
                    content: "本笔记包含100个最常用的英语词汇及其中文释义。",
                    createdAt: "2023-04-16T14:20:00"
                },
                {
                    id: 3,
                    title: "微积分公式大全",
                    category: "math",
                    content: "常见微积分公式和证明过程的总结。",
                    createdAt: "2023-04-17T09:45:00"
                }
            ];
            
            renderNotes(dummyNotes);
        }
    });
}

// Load notes by category
function loadNotesByCategory(category) {
    $.ajax({
        url: '/api/study-notes/category/' + category,
        type: 'GET',
        success: function(data) {
            renderNotes(data);
        },
        error: function(xhr) {
            console.error('Error loading notes by category:', xhr);
            
            // For demonstration, filter dummy data if the API is not available
            loadNotes();
        }
    });
}

// Search notes
function searchNotes(term) {
    $.ajax({
        url: '/api/study-notes/search?title=' + encodeURIComponent(term),
        type: 'GET',
        success: function(data) {
            renderNotes(data);
        },
        error: function(xhr) {
            console.error('Error searching notes:', xhr);
            
            // For demonstration, show a message if the API is not available
            $('#studyNotesList').html('<div class="col-12"><p class="text-center">搜索功能需要服务器支持。请尝试使用分类过滤器。</p></div>');
        }
    });
}

// Create a new note
function createNote(note) {
    console.log('Creating note with data:', note);
    $.ajax({
        url: '/api/study-notes',
        type: 'POST',
        contentType: 'application/json',
        data: JSON.stringify(note),
        success: function(data) {
            $('#createNoteModal').modal('hide');
            showToast('笔记创建成功！');
            loadNotes();
        },
        error: function(xhr, status, error) {
            console.error('Error creating note:', xhr, status, error);
            console.log('Response Text:', xhr.responseText);
            
            // 显示具体错误消息
            let errorMsg = '笔记创建失败';
            try {
                if (xhr.responseJSON && xhr.responseJSON.message) {
                    errorMsg += ': ' + xhr.responseJSON.message;
                } else if (xhr.responseText) {
                    const response = JSON.parse(xhr.responseText);
                    if (response.message) {
                        errorMsg += ': ' + response.message;
                    }
                } else if (xhr.statusText) {
                    errorMsg += ': ' + xhr.statusText;
                }
            } catch (e) {
                console.error('Error parsing error response:', e);
                errorMsg += ': ' + xhr.statusText || error;
            }
            showToast(errorMsg);
        }
    });
}

// Update an existing note
function updateNote(noteId, note) {
    console.log('Updating note ID:', noteId, 'with data:', note);
    $.ajax({
        url: '/api/study-notes/' + noteId,
        type: 'PUT',
        contentType: 'application/json',
        data: JSON.stringify(note),
        success: function(data) {
            $('#createNoteModal').modal('hide');
            showToast('笔记更新成功！');
            loadNotes();
        },
        error: function(xhr, status, error) {
            console.error('Error updating note:', xhr, status, error);
            console.log('Response Text:', xhr.responseText);
            
            // 显示具体错误消息
            let errorMsg = '笔记更新失败';
            try {
                if (xhr.responseJSON && xhr.responseJSON.message) {
                    errorMsg += ': ' + xhr.responseJSON.message;
                } else if (xhr.responseText) {
                    const response = JSON.parse(xhr.responseText);
                    if (response.message) {
                        errorMsg += ': ' + response.message;
                    }
                } else if (xhr.statusText) {
                    errorMsg += ': ' + xhr.statusText;
                }
            } catch (e) {
                console.error('Error parsing error response:', e);
                errorMsg += ': ' + xhr.statusText || error;
            }
            showToast(errorMsg);
        }
    });
}

// Delete a note
function deleteNote(noteId) {
    $.ajax({
        url: '/api/study-notes/' + noteId,
        type: 'DELETE',
        success: function() {
            $('#viewNoteModal').modal('hide');
            loadNotes();
        },
        error: function(xhr) {
            console.error('Error deleting note:', xhr);
            
            // For demonstration, show success message if the API is not available
            alert('笔记删除功能需要服务器支持。');
            $('#viewNoteModal').modal('hide');
        }
    });
}

// View a note
function viewNote(noteId) {
    $.ajax({
        url: '/api/study-notes/' + noteId,
        type: 'GET',
        success: function(data) {
            $('#viewNoteTitle').text(data.title);
            $('#viewNoteCategory').text(data.category);
            $('#viewNoteContent').html(data.content);
            
            // Set the noteId to the edit and delete buttons
            $('#editNoteBtn').data('id', data.id);
            $('#deleteNoteBtn').data('id', data.id);
            
            $('#viewNoteModal').modal('show');
        },
        error: function(xhr) {
            console.error('Error loading note details:', xhr);
            
            // For demonstration, use dummy data if the API is not available
            const dummyNote = {
                id: noteId,
                title: '示例笔记 ' + noteId,
                category: 'Programming',
                content: '这是一条示例笔记内容。实际应用中，这里将显示从服务器获取的笔记详情。',
                createdAt: new Date().toISOString()
            };
            
            $('#viewNoteTitle').text(dummyNote.title);
            $('#viewNoteCategory').text(dummyNote.category);
            $('#viewNoteContent').html(dummyNote.content);
            
            // Set the noteId to the edit and delete buttons
            $('#editNoteBtn').data('id', dummyNote.id);
            $('#deleteNoteBtn').data('id', dummyNote.id);
            
            $('#viewNoteModal').modal('show');
        }
    });
}

// Render notes to the UI
function renderNotes(notes) {
    $('#studyNotesList').empty();
    
    if (notes.length === 0) {
        $('#studyNotesList').html('<div class="col-12"><p class="text-center">没有找到笔记。请尝试不同的筛选条件或创建新笔记。</p></div>');
        return;
    }
    
    $.each(notes, function(index, note) {
        const categoryText = getCategoryText(note.category);
        const createdAt = formatDate(note.createdAt);
        
        const noteCard = `
            <div class="col-md-6 col-lg-4 mb-4">
                <div class="card note-card h-100" data-id="${note.id}">
                    <div class="card-body">
                        <h5 class="card-title">${note.title}</h5>
                        <span class="badge bg-primary mb-2">${categoryText}</span>
                        <p class="card-text">${note.content}</p>
                    </div>
                    <div class="card-footer text-muted">
                        创建于 ${createdAt}
                    </div>
                </div>
            </div>
        `;
        
        $('#studyNotesList').append(noteCard);
    });
}

// Get category display text based on category code
function getCategoryText(category) {
    const categories = {
        'programming': '编程',
        'language': '语言',
        'math': '数学',
        'science': '科学',
        'other': '其他'
    };
    
    return categories[category.toLowerCase()] || category;
}

// Format date to a readable string
function formatDate(dateString) {
    try {
        const date = new Date(dateString);
        if (isNaN(date.getTime())) {
            console.error('Invalid date:', dateString);
            return 'Invalid Date';
        }
        return date.toLocaleDateString('zh-CN', {
            year: 'numeric', 
            month: 'short', 
            day: 'numeric',
            hour: '2-digit',
            minute: '2-digit'
        });
    } catch (e) {
        console.error('Error formatting date:', e);
        return dateString || 'Unknown Date';
    }
}

// Display a toast message
function showToast(message) {
    // Create toast container if it doesn't exist
    if ($('#toastContainer').length === 0) {
        $('body').append('<div id="toastContainer" style="position: fixed; top: 20px; right: 20px; z-index: 9999;"></div>');
    }
    
    // Create unique ID for this toast
    const toastId = 'toast-' + Date.now();
    
    // Create toast HTML
    const toast = `
    <div id="${toastId}" class="toast show" role="alert" aria-live="assertive" aria-atomic="true" data-bs-delay="3000">
        <div class="toast-header">
            <strong class="me-auto">通知</strong>
            <button type="button" class="btn-close" data-bs-dismiss="toast" aria-label="Close"></button>
        </div>
        <div class="toast-body">${message}</div>
    </div>`;
    
    // Add toast to container
    $('#toastContainer').append(toast);
    
    // Initialize Bootstrap toast
    const toastElement = new bootstrap.Toast(document.getElementById(toastId));
    
    // Auto hide after 3 seconds
    setTimeout(() => {
        toastElement.hide();
        // Remove from DOM after hiding
        setTimeout(() => {
            $(`#${toastId}`).remove();
        }, 500);
    }, 3000);
} 