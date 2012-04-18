from fabric.api import *

env.use_ssh_config = True
env.hosts = ['feynman']

@task
def submit_job():
  rebuild_project(branch="deploy")      

@task
def rebuild_project(branch, project_dir="$UCN_DIR", remote="origin", make_clean=False):
  with cd(project_dir):
    # -- Update code from remote
    run("git checkout {br}".format(br=branch))
    run("git pull --ff-only {re} {br}".format(re=remote, br=branch))

    with cd("build"):
      # -- Run a 'make clean' if chosen 
      if make_clean:
        run("make clean")
      # -- Run the build 
      run("cmake ..")
      run("make")




   #1. copy run config directory to scratch
   #2. check that global environment is setup right
   #3. generate neutrons for run
   #4. check that config files are valid
   #5. check that all files are present
   #6. submit job to queue
