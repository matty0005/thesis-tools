import requests

def fetch_url_content(url):
    try:
        response = requests.get(url)
        response.raise_for_status()  # Raise an exception for HTTP errors
        
        return response.text
    except requests.RequestException as e:
        print(f"An error occurred: {e}")
        return None

if __name__ == "__main__":
    url = "http://10.20.1.120/assets/index-cc53c881.js"
    # url = "http://10.20.1.120/assets/index-e507f491.css"
    content = fetch_url_content(url)
    if content:
        print(content)
