$(document).ready(function() {
    // Mock data for study progress
    const mockStudyProgress = {
        totalPoints: 850,
        currentStreak: 7,
        wordsLearnedToday: 8,
        totalWordsGoal: 10,
        dailyWords: [
            { day: 'Day 1', words: 7 },
            { day: 'Day 2', words: 9 },
            { day: 'Day 3', words: 6 },
            { day: 'Day 4', words: 10 },
            { day: 'Day 5', words: 8 },
            { day: 'Day 6', words: 10 },
            { day: 'Today', words: 8 } // Reflects wordsLearnedToday
        ]
    };

    // Populate overall stats
    $('#totalPoints').text(mockStudyProgress.totalPoints);
    $('#currentStreak').text(mockStudyProgress.currentStreak);
    $('#wordsLearnedToday').text(mockStudyProgress.wordsLearnedToday);
    $('#totalWordsGoal').text(mockStudyProgress.totalWordsGoal);

    // Render daily words chart
    renderDailyWordsChart(mockStudyProgress.dailyWords);

    function renderDailyWordsChart(data) {
        const $chartContainer = $('#dailyWordsChart');
        $chartContainer.empty();

        const maxWords = Math.max(...data.map(item => item.words));
        const chartHeight = 200; // Defined in CSS

        data.forEach(item => {
            const barHeight = (item.words / maxWords) * chartHeight; // Scale height dynamically
            const barHtml = `
                <div class="bar" style="height: ${barHeight}px;">
                    <span class="bar-value">${item.words}</span>
                    <span class="bar-label">${item.day}</span>
                </div>
            `;
            $chartContainer.append(barHtml);
        });
    }
}); 