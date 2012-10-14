from os import *
from fabric.api import *

env.use_ssh_config = True
env.hosts = ['feynman']

@task
def submit_job(path_to_run, queue, config="config.cfg"):
  push_config_files(path_to_run)
  rebuild_project("develop")
  generate_initial_particles(path_to_run)
  build_geometries(path_to_run)
  # Submit job
  remote_runs_dir = run("echo $UCN_RUNS")
  remote_dir = "{runs_dir}/{path}/".format(runs_dir=remote_runs_dir, path=path_to_run)
  with cd(remote_dir):
    run("batch_simulate {cfg} {qu}".format(cfg=config, qu=queue))

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

@task
def push_config_files(path_to_run):
  server = env.hosts[0]
  # Define which files are to be copied
  filter_patterns = ["--filter '+ *.cfg'", "--filter '+ *.C'", "--filter '+ *.dta'", "--filter '+ *.txt'", "--filter '- *'"]
  filter_patterns = " ".join(filter_patterns)
  local_runs_dir = environ['UCN_RUNS']
  local_dir = "{0}/{1}/".format(local_runs_dir, path_to_run)
  remote_runs_dir = run("echo $UCN_RUNS")
  remote_dir = "{runs_dir}/{path}/".format(runs_dir=remote_runs_dir, path=path_to_run)
  # Create full path to remote directory if it doesn't exist already
  run("mkdir -p {0}".format(remote_dir))
  # Define rsync options
  rsync_options = "-avz --delete --force "
  # -- Rsync folder to server
  local("rsync {opt} {files} {local} {serv}:{remote}".format(opt=rsync_options, files=filter_patterns, local=local_dir, serv=server, remote=remote_dir))

@task
def generate_initial_particles(path_to_run, show_plots='false'):
  remote_runs_dir = run("echo $UCN_RUNS")
  remote_dir = "{runs_dir}/{path}/".format(runs_dir=remote_runs_dir, path=path_to_run)
  with cd(remote_dir):
    run("generate_ucn --config=config.cfg --plot={0}".format(show_plots));

@task
def build_geometries(path_to_run, geom_macro='*.C'):
  remote_runs_dir = run("echo $UCN_RUNS")
  remote_dir = "{runs_dir}/{path}/".format(runs_dir=remote_runs_dir, path=path_to_run)
  with cd(remote_dir):
    run("root -l -b -q {0}".format(geom_macro));
 
@task
def list_jobs(username="mb325"):
  with settings(hide('warnings'), warn_only=True):
    result = run("qstat -u {}".format(username))
  if result.failed:
    print "No jobs still running"
