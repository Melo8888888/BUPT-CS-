$(document).ready(function() {
    let gameWords = [];
    let selectedWord = null;
    let selectedDefinition = null;
    let matchedPairs = 0;

    loadGameWords();

    function loadGameWords() {
        // For the game, fetch a smaller set, e.g., 5 words.
        // In a real application, these might be unlearned words or words for review.
        $.ajax({
            url: '/api/cet6words/daily/5', // Fetch 5 words for the game
            type: 'GET',
            success: function(words) {
                if (words && words.length >= 5) {
                    gameWords = words;
                    initializeGame();
                } else {
                    console.warn('API /api/cet6words/daily/5 returned insufficient words. Using dummy data for game.');
                    // Fallback to dummy data if API doesn't provide enough words
                    gameWords = [
                        { id: 1, word: 'ubiquitous', definition: '普遍存在的' },
                        { id: 2, word: 'meticulous', definition: '一丝不苟的' },
                        { id: 3, word: 'benevolent', definition: '仁慈的' },
                        { id: 4, word: 'ephemeral', definition: '短暂的' },
                        { id: 5, word: 'gregarious', definition: '好交际的' },
                        { id: 6, word: 'paradox', definition: '悖论' },
                        { id: 7, word: 'serendipity', definition: '机缘巧合' },
                        { id: 8, word: 'cacophony', definition: '刺耳的嘈杂声' },
                        { id: 9, word: 'equivocal', definition: '模棱两可的' },
                        { id: 10, word: 'plethora', definition: '过多' },
                        { id: 11, word: 'ostentatious', definition: '炫耀的' },
                        { id: 12, word: 'perfunctory', definition: '敷衍的' },
                        { id: 13, word: 'capricious', definition: '反复无常的' },
                        { id: 14, word: 'dogmatic', definition: '教条的' },
                        { id: 15, word: 'reclusive', definition: '隐居的' },
                        { id: 16, word: 'superfluous', definition: '多余的' },
                        { id: 17, word: 'alacrity', definition: '敏捷' },
                        { id: 18, word: 'platitude', definition: '陈词滥调' },
                        { id: 19, word: 'innocuous', definition: '无害的' },
                        { id: 20, word: 'sycophant', definition: '马屁精' }
                    ];
                    initializeGame();
                }
            },
            error: function(xhr, status, error) {
                console.error('Error fetching game words:', xhr, status, error);
                gameWords = [
                    { id: 1, word: 'ubiquitous', definition: '普遍存在的' },
                    { id: 2, word: 'meticulous', definition: '一丝不苟的' },
                    { id: 3, word: 'benevolent', definition: '仁慈的' },
                    { id: 4, word: 'ephemeral', definition: '短暂的' },
                    { id: 5, word: 'gregarious', definition: '好交际的' },
                    { id: 6, word: 'paradox', definition: '悖论' },
                    { id: 7, word: 'serendipity', definition: '机缘巧合' },
                    { id: 8, word: 'cacophony', definition: '刺耳的嘈杂声' },
                    { id: 9, word: 'equivocal', definition: '模棱两可的' },
                    { id: 10, word: 'plethora', definition: '过多' },
                    { id: 11, word: 'ostentatious', definition: '炫耀的' },
                    { id: 12, word: 'perfunctory', definition: '敷衍的' },
                    { id: 13, word: 'capricious', definition: '反复无常的' },
                    { id: 14, word: 'dogmatic', definition: '教条的' },
                    { id: 15, word: 'reclusive', definition: '隐居的' },
                    { id: 16, word: 'superfluous', definition: '多余的' },
                    { id: 17, word: 'alacrity', definition: '敏捷' },
                    { id: 18, word: 'platitude', definition: '陈词滥调' },
                    { id: 19, word: 'innocuous', definition: '无害的' },
                    { id: 20, word: 'sycophant', definition: '马屁精' }
                ];
                initializeGame();
            }
        });
    }

    function initializeGame() {
        const $wordList = $('#wordList');
        const $definitionList = $('#definitionList');
        $wordList.empty();
        $definitionList.empty();
        $('#matchLineContainer').empty();
        matchedPairs = 0;

        const definitions = gameWords.map(w => ({ id: w.id, definition: w.definition }));
        shuffleArray(definitions);

        gameWords.forEach(word => {
            $wordList.append(`<li class="word-item" data-id="${word.id}">${word.word}</li>`);
        });

        definitions.forEach(def => {
            $definitionList.append(`<li class="definition-item" data-id="${def.id}">${def.definition}</li>`);
        });

        addEventListeners();
    }

    function addEventListeners() {
        $('.word-item').click(function() {
            if ($(this).hasClass('matched')) return; // Already matched

            $('.word-item').removeClass('selected');
            $(this).addClass('selected');
            selectedWord = $(this);
            checkMatch();
        });

        $('.definition-item').click(function() {
            if ($(this).hasClass('matched')) return; // Already matched

            $('.definition-item').removeClass('selected');
            $(this).addClass('selected');
            selectedDefinition = $(this);
            checkMatch();
        });
    }

    function checkMatch() {
        if (selectedWord && selectedDefinition) {
            const wordId = selectedWord.data('id');
            const definitionId = selectedDefinition.data('id');

            if (wordId === definitionId) {
                // Match found!
                selectedWord.addClass('matched').removeClass('selected');
                selectedDefinition.addClass('matched').removeClass('selected');
                drawMatchLine(selectedWord, selectedDefinition);
                matchedPairs++;

                if (matchedPairs === gameWords.length) {
                    setTimeout(showCongratulations, 500);
                }
            } else {
                // No match, remove selection after a brief delay
                setTimeout(() => {
                    selectedWord.removeClass('selected');
                    selectedDefinition.removeClass('selected');
                }, 500);
            }
            selectedWord = null;
            selectedDefinition = null;
        }
    }

    function drawMatchLine($wordElem, $defElem) {
        const wordPos = $wordElem.offset();
        const defPos = $defElem.offset();

        const wordCenterX = wordPos.left + $wordElem.outerWidth();
        const wordCenterY = wordPos.top + $wordElem.outerHeight() / 2;
        const defCenterX = defPos.left;
        const defCenterY = defPos.top + $defElem.outerHeight() / 2;

        const containerOffset = $('#matchLineContainer').offset();
        const startX = wordCenterX - containerOffset.left;
        const startY = wordCenterY - containerOffset.top;
        const endX = defCenterX - containerOffset.left;
        const endY = defCenterY - containerOffset.top;

        const length = Math.sqrt(Math.pow(endX - startX, 2) + Math.pow(endY - startY, 2));
        const angle = Math.atan2(endY - startY, endX - startX) * 180 / Math.PI;

        const $line = $('<div>')
            .addClass('match-line')
            .css({
                'left': startX + 'px',
                'top': startY + 'px',
                'width': length + 'px',
                'transform': `rotate(${angle}deg)`,
                'background-color': getRandomColor()
            });
        $('#matchLineContainer').append($line);
    }

    function getRandomColor() {
        const colors = ['#007bff', '#28a745', '#ffc107', '#dc3545', '#6f42c1', '#fd7e14', '#20c997'];
        return colors[Math.floor(Math.random() * colors.length)];
    }

    function shuffleArray(array) {
        for (let i = array.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [array[i], array[j]] = [array[j], array[i]];
        }
    }

    function showCongratulations() {
        $('#congratulationsModal').css('display', 'flex');
        // Add confetti effect
        for (let i = 0; i < 50; i++) {
            createConfetti();
        }

        // Add event listener for Play Again button
        $('#playAgainBtn').off('click').on('click', function() {
            $('#congratulationsModal').hide();
            initializeGame();
        });
    }

    function createConfetti() {
        const confetti = document.createElement('div');
        confetti.classList.add('confetti');
        confetti.style.left = Math.random() * 100 + 'vw';
        confetti.style.backgroundColor = getRandomColor();
        confetti.style.animationDuration = (Math.random() * 2 + 1) + 's'; // 1-3 seconds
        confetti.style.animationDelay = (Math.random() * 0.5) + 's';
        document.body.appendChild(confetti);

        // Remove confetti after animation
        confetti.addEventListener('animationend', () => {
            confetti.remove();
        });
    }
}); 