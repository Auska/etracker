function showForm() {
    $('#settingsForm').fadeIn(300);
    $('#loading').fadeOut(300);
}

function showLoading() {
    $('#settingsForm').fadeOut(300);
    $('#loading').fadeIn(300);
}

function showStatus(message, type) {
    const statusEl = $('#statusMessage');
    statusEl.removeClass('success error').addClass(type).text(message).fadeIn(300);

    setTimeout(function() {
        statusEl.fadeOut(300);
    }, 5000);
}

function update(data) {
    $.ajax({
        url: "/api/set",
        dataType: "json",
        data: data,
        success: function (response) {
            $.each(response, function (key) {
                const input = $('input[name=' + key + '], select[name=' + key + ']');
                input.val(response[key]);
            });

            showForm();
            if (Object.keys(data).length > 0) {
                showStatus('✓ Settings saved successfully!', 'success');
            }
        },
        error: function(xhr, status, error) {
            showForm();
            if (xhr.status === 401) {
                showStatus('✗ Authentication failed. Please check your credentials.', 'error');
            } else if (xhr.status === 400) {
                showStatus('✗ Invalid request. Please try again.', 'error');
            } else {
                showStatus('✗ Failed to save settings. Please try again.', 'error');
            }
            console.error('Settings update error:', error);
        }
    });
}

$(document).ready(function () {
    const form = $('#settingsForm');

    // Load current settings
    update({});

    // Handle form submission
    form.on('submit', function (e) {
        e.preventDefault();
        showLoading();

        const data = {};
        form.find('input[name], select[name]').each(function () {
            const that = $(this);
            const value = that.val();
            data[that.attr('name')] = value;
        });

        update(data);
    });

    // Handle reset button
    $('#resetBtn').on('click', function () {
        if (confirm('Are you sure you want to reset to default values?')) {
            form.find('input[name="interval"]').val('1799');
            form.find('input[name="max_peers_response"]').val('60');
            form.find('input[name="socket_timeout"]').val('3');
            form.find('select[name="keep_alive"]').val('1');
            showStatus('✓ Reset to default values. Click "Save Changes" to apply.', 'success');
        }
    });

    // Validate input on blur
    form.find('input[type="number"]').on('blur', function () {
        const input = $(this);
        const min = parseInt(input.attr('min'));
        const max = parseInt(input.attr('max'));
        const value = parseInt(input.val());

        if (isNaN(value) || value < min || value > max) {
            input.css('border-color', '#dc3545');
            showStatus(`✗ Value must be between ${min} and ${max}`, 'error');
        } else {
            input.css('border-color', '#28a745');
        }
    });

    // Clear validation on focus
    form.find('input').on('focus', function () {
        $(this).css('border-color', '#e0e0e0');
    });
});
