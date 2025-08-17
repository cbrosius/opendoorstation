document.addEventListener('DOMContentLoaded', function() {
    const form = document.getElementById('config-form');
    const status = document.getElementById('status');

    // Fetch current config and populate the form
    status.textContent = 'Loading configuration...';
    fetch('/api/config')
        .then(response => {
            if (!response.ok) {
                throw new Error('Failed to fetch config');
            }
            return response.json();
        })
        .then(data => {
            document.getElementById('wifi_ssid').value = data.wifi_ssid || '';
            document.getElementById('sip_user').value = data.sip_user || '';
            document.getElementById('sip_domain').value = data.sip_domain || '';
            document.getElementById('sip_callee_uri').value = data.sip_callee_uri || '';
            status.textContent = '';
        })
        .catch(error => {
            console.error('Error fetching config:', error);
            status.textContent = 'Error loading configuration.';
            status.style.color = 'red';
        });

    // Handle form submission
    form.addEventListener('submit', function(event) {
        event.preventDefault();
        status.textContent = 'Saving...';
        status.style.color = 'black';

        const formData = {
            wifi_ssid: document.getElementById('wifi_ssid').value,
            sip_user: document.getElementById('sip_user').value,
            sip_domain: document.getElementById('sip_domain').value,
            sip_callee_uri: document.getElementById('sip_callee_uri').value,
        };

        fetch('/api/config', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(formData),
        })
        .then(response => {
            if (response.ok) {
                status.textContent = 'Configuration saved! The device will now reboot.';
                status.style.color = 'green';
                // The ESP32 should handle the reboot after sending a success response.
            } else {
                response.text().then(text => {
                    throw new Error('Failed to save configuration: ' + text);
                });
            }
        })
        .catch(error => {
            console.error('Error saving config:', error);
            status.textContent = 'Error saving configuration.';
            status.style.color = 'red';
        });
    });
});
