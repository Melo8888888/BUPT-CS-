$(document).ready(function() {
    // Initialize any common functionality

    // Set active nav item based on current page
    const currentPath = window.location.pathname;
    
    $('.navbar-nav .nav-link').each(function() {
        const navPath = $(this).attr('href');
        if (currentPath === navPath || 
            (currentPath === '/' && navPath === '/') ||
            (currentPath !== '/' && navPath !== '/' && currentPath.startsWith(navPath))) {
            $(this).addClass('active');
        } else {
            $(this).removeClass('active');
        }
    });
    
    // Initialize tooltips if used
    const tooltipTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="tooltip"]'));
    const tooltipList = tooltipTriggerList.map(function (tooltipTriggerEl) {
        return new bootstrap.Tooltip(tooltipTriggerEl);
    });
    
    // Add hover effect to cards on the home page
    $('.card').on('mouseenter', function() {
        $(this).addClass('shadow-lg');
    }).on('mouseleave', function() {
        $(this).removeClass('shadow-lg');
    });
}); 