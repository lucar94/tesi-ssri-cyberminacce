document.getElementById('formLogin').addEventListener('submit', async function(event) {
  event.preventDefault();

  const form = event.target;
  const formData = new FormData(form);

  const response = await fetch('/api/login', {
        method: 'POST',
        headers:{
              'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: new URLSearchParams(formData)
  });

  if (response.redirected) {
    window.location.href = response.url;
  }

  if(response.status === 401) {
    alert("Invalid username or password");
  }
});