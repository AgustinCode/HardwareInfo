from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from webdriver_manager.chrome import ChromeDriverManager
import time


class LinkedinDriver: #Using Selenium due some issues with BeautifulSoup

    def _setup_driver(self):
        chrome_options = Options()
        chrome_options.add_argument("--disable-notifications")
        chrome_options.add_argument("--disable-extensions")
        chrome_options.add_argument("--force-device-scale-factor=0.6") 
        chrome_options.add_argument("--lang=en-US")

        service = Service(ChromeDriverManager().install())
        self.driver = webdriver.Chrome(service=service, options=chrome_options)
        self.wait = WebDriverWait(self.driver, 10)


    def __init__(self, email, password):
        self.email = email
        self.password = password
        self._setup_driver()
        self.job_urls = []
        self.job_titles = []
        self.collected = len(self.job_urls)


    def login(self):
        self.driver.get("https://www.linkedin.com/login/")

        email_field = self.wait.until(EC.presence_of_element_located((By.ID, "username")))
        email_field.send_keys(self.email, Keys.ENTER)
        psw_field = self.driver.find_element(By.ID, "password")
        psw_field.send_keys(self.password, Keys.ENTER)
    

    def search_easy_apply_jobs(self):
        self.driver.get("https://www.linkedin.com/jobs/search/?currentJobId=3971455036&f_AL=true&origin=JOB_SEARCH_PAGE_JOB_FILTER")
        time.sleep(5)

    def collect_jobs(self):
        # Execute JavaScript to extract job links and titles
        jobs = self.driver.execute_script("""
            let jobElements = document.querySelectorAll('ul.scaffold-layout__list-container > li');
            let jobs = [];
            jobElements.forEach((job) => {
                let titleElement = job.querySelector('a.job-card-list__title');
                let linkElement = job.querySelector('a.job-card-list__title');
                if (titleElement && linkElement) {
                    let jobTitle = titleElement.innerText;
                    let jobLink = linkElement.href;
                    jobs.push({title: jobTitle, link: jobLink});
                }
            });
            return jobs;
        """)

        for job in jobs:
            self.job_titles.append(job['title'])
            self.job_urls.append(job['link'])

        return jobs
        

    def close(self):
        if self.driver:
            self.driver.quit()



    