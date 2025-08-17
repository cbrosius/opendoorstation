document.addEventListener('DOMContentLoaded', function() {
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

    // --- Configuration Display ---
    function loadConfiguration() {
        fetch('/api/config')
            .then(response => {
                if (!response.ok) {
                    throw new Error('Failed to fetch config');
                }
                return response.json();
            })
            .then(data => {
                document.getElementById('wifi_ssid').textContent = data.wifi_ssid || 'N/A';
                document.getElementById('sip_user').textContent = data.sip_user || 'N/A';
                document.getElementById('sip_domain').textContent = data.sip_domain || 'N/A';
                document.getElementById('sip_callee_uri').textContent = data.sip_callee_uri || 'N/A';
            })
            .catch(error => {
                console.error('Error fetching config:', error);
                document.getElementById('wifi_ssid').textContent = 'Error';
            });
    }

    loadConfiguration();
});