#!/bin/sh
#SBATCH --job-name=vic_Stehekin_Test
#SBATCH -o vic_test.out
#SBATCH -e vic_test.err
#SBATCH -n 1
#SBATCH -w node08
export MYAPP=/home/johsch/VIC_AMBHAS2/VIC_AMBHAS/VIC/vic/drivers/image/vic_image.exe

export MPICH_PROCESS_GROUP=no
export I_MPI_FABRICS="shm:tmi"
export OMP_NUM_THREADS=1

# ---------------------------
# set up the mpich  and netcdf versions to use
# ---------------------------
# load the module
. /etc/profile.d/modules.sh
module purge
module load shared
module load openmpi/gcc/64/1.10.1
module load netcdf/gcc/64/4.4.1
# ---------------------------
# run the job
# ---------------------------
echo "Will run command: mpirun $MYAPP, using $SLURM_NTASKS cores"
mpirun $MYAPP -g /home/johsch/VIC_AMBHAS2/VIC_AMBHAS/example/VIC_sample_data/image/Stehekin/parameters/Stehekin_image_test.global.txt

