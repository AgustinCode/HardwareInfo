from classes.LDscrapper import LinkedinDriver
import time

def main():
    ldriver = LinkedinDriver("relaxingscript@gmail.com", "testtest123")
    try:
        ldriver.login()
        ldriver.search_easy_apply_jobs()
        jobs = ldriver.collect_jobs()
        for i in range (len(jobs)):
            print(f"Job {i+1}",ldriver.job_titles[i], "\n")
    
    except Exception as e:
        print(f"Error logging in. ERROR {e}")
    finally:
        ldriver.close()

if __name__ == "__main__":
    main()