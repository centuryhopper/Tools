#! /home/leo_zhang/miniconda3/envs/web_auto/bin/python


from github import Github
import datetime
import pwd
import os, argparse
from dotenv import load_dotenv
load_dotenv()

def main():
    g = Github(os.getenv('github_api_token'))

    # repos = g.search_repositories(query='language:python')
    # for repo in g.get_user().get_repos():
    #     print(repo.name)

    parser = argparse.ArgumentParser()
    parser.add_argument("--name", "-n", type=str,dest="name",required=True)
    parser.add_argument("--private", "-p", dest="isPrivate",action='store_true')
    args = parser.parse_args()
    repoName = args.name
    isPrivate = args.isPrivate

    # create a repo
    user = g.get_user()
    repo = user.create_repo(repoName,private=isPrivate)

    # creating local repository and connect with the created remote one from above
    try:
        REPO_PATH = f'/home/{pwd.getpwuid(os.getuid()).pw_name}/Documents/GitHub' # change this line to be your desired local repo path
        os.chdir(REPO_PATH)
        os.system(f'mkdir {repoName}')
        os.chdir(os.path.join(REPO_PATH, repoName))
        os.system('git init')
        os.system(f'git remote add origin git@github.com:leozhang1/{repoName}.git') # change 'leozhang1' to your own github username
    #region files you want to add
        os.system(f"echo # {repoName} >> README.md")
        
        license_text =f"""
MIT License
Copyright (c) {datetime.datetime.now().year} Leo Zhang
        
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
        
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""
        
        with open("LICENSE.txt", "w") as file:
            file.write(license_text)
        os.system("touch .gitignore")
        os.system('git add .')
        os.system('git commit -m "Initial Commit"')
        os.system('git branch -m main')
        os.system('git push -u origin main')
        #os.system('git checkout -b main_laptop')
        #os.system('git push -u origin main_laptop')

    #endregion


    except FileExistsError as f:
        print(f)
    except Exception as e:
        print(e)


if __name__ == '__main__':
    pass
    main()



