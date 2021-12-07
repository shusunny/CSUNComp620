#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <mpi.h>

#define SIZE 800

int mandel(complex z0){
  int i;
  complex z;

  z=z0;
  for(i=1;i<320;i++){
    z=z*z+z0;
    if ((creal(z)*creal(z))+(cimag(z)*cimag(z))>4.0) break;
  }

  return i;
}


int main(){
  double xmin,xmax,ymin,ymax;
  int i,j,rows,columns,rank,nproc,msg,nslaves;
  complex z;
  int row[SIZE],hdr,r,s;
  unsigned char line[3*SIZE], *buff;
  FILE *img;
  MPI_Status st;

  MPI_Init(NULL,NULL);
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  nslaves=nproc-1;

  xmin=-2; xmax=-1;
  ymin=0; ymax=1;

  if (rank==0){ /* Master */
    img=fopen("mandel.pam","w");
    hdr=fprintf(img,"P6\n%d %d 255\n",SIZE,SIZE);

    for(i=0;i<nslaves;i++)
      MPI_Send(&i,1,MPI_INT,i+1,0,MPI_COMM_WORLD);
      
    for (i=0;i<SIZE;i++){
       MPI_Recv(line,3*SIZE,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,
                MPI_COMM_WORLD,&st);

       r=st.MPI_TAG;
       s=st.MPI_SOURCE;
       fseek(img,hdr+3*SIZE*r,SEEK_SET);
       fwrite(line,1,3*SIZE,img);

       if ((i+nslaves)<SIZE) msg=i+nslaves;
       else msg=-1;
                            
       MPI_Send(&msg,1,MPI_INT,s,0,MPI_COMM_WORLD);
    }

  }
  else{  /* Slave */

    for(;;){
      MPI_Recv(&i,1,MPI_INT,0,0,MPI_COMM_WORLD,&st);

      if (i==-1) break;

      for(j=0;j<SIZE;j++){
        z=xmin+j*((xmax-xmin)/SIZE)+(ymax-i*((ymax-ymin)/SIZE))*I;
        row[j]=mandel(z);
      }

      for(j=0;j<SIZE;j++){
        if (row[j]<=63){
          line[3*j]=255;
          line[3*j+1]=line[3*j+2]=255-4*row[j];
        }
        else{
          line[3*j]=255;
          line[3*j+1]=row[j]-63;
          line[3*j+2]=0;
        }
        if (row[j]==320) line[3*j]=line[3*j+1]=line[3*j+2]=255;
      }

      MPI_Send(line,3*SIZE,MPI_CHAR,0,i,MPI_COMM_WORLD);

    }
  } /* End if master / slave */

  MPI_Finalize();
  return 0;
}