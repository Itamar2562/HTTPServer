const form = document.getElementById('RegisterForm');

form.addEventListener('submit', async (event) => {
    event.preventDefault();

    const formData = new URLSearchParams(new FormData(form));

    try {
        const response = await fetch('/Register', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded'
            },
            body: formData.toString()
        });

        if (response.ok) {
            const result = await response.json();
            console.log('Success:', result);
        } else {
            console.error('Server error:', response.statusText);
        }
    } catch (error) {
        console.error('Network error:', error);
    }
});