document.addEventListener('DOMContentLoaded', function() {
    const form = document.getElementById('config-form');
    const status = document.getElementById('status');
    const virtualButton = document.getElementById('virtual-button');
    const doorStatus = document.getElementById('door-status');
    const lightStatus = document.getElementById('light-status');

    // --- Virtual I/O Handling ---
    virtualButton.addEventListener('click', function() {
        fetch('/api/virtual/button', { method: 'POST' })
            .catch(console.error);
    });

    function pollRelayStatus() {
        fetch('/api/virtual/relays')
            .then(response => response.json())
            .then(data => {
                doorStatus.classList.toggle('active', data.door_active);
                lightStatus.classList.toggle('active', data.light_active);
            })
            .catch(console.error);
    }

    setInterval(pollRelayStatus, 1000); // Poll every second


    // --- Configuration Form Handling ---

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
