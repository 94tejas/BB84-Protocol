#include<stdio.h>
#include<math.h>
#include<malloc.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define MAX 10000

struct stack
{
	int key[MAX];
	int count;
}final;

void encode_X_dir(int* alice_str,double** en_alice_str, int i);
void encode_Z_dir(int* alice_str, double** en_alice_str, int i);
void measure_X_dir(double** en_str,int* str,int i);
void measure_Z_dir(double** en_str,int* str,int i);
void Quantum_channel(double** en_alice_str,double** en_bob_str,double** en_eve_str,int* bob_str,int* eve_str,char* bob,int N);
int Parity(int* block,int l);
void paritycheck(int* alice_block,int* bob_block,struct stack final, int l);


int main()
{
	int* alice_str;
	int N,i,k;
	srand(time(NULL));


	//ALICE GENERATES RANDOM NUMBER SEQUENCE OF 1s AND 0s
	printf("Enter the no. of char in the string\n");
	scanf("%d",&N);
	alice_str = (int*)malloc(N*sizeof(int*));
	for(k=0;k<N;k++)
	{
		alice_str[k]=rand()%2;
		printf("%d",alice_str[k]);
	}
	printf("\n");


	//ALICE ENCODES EACH NUMBER OF THE SEQUENCE IN EITHER X OR Z DIRECTION RANDOMLY
	double** en_alice_str;
	int temp;
	char alice[N];
	en_alice_str=(double**)malloc(N*sizeof(double*));
	for(i=0;i<N;i++)
	{
		en_alice_str[i]=(double*)malloc(2*sizeof(double));
		temp = rand()%2;
		if(temp==0)
		{
			encode_X_dir(alice_str,en_alice_str,i);
			alice[i]='x';
		}
		else
		{
			encode_Z_dir(alice_str,en_alice_str,i);
			alice[i]='z';
		}
		printf("alice encoded %lf %lf\n",en_alice_str[i][0],en_alice_str[i][1]);
	}


	//ALICE SENDS ENCODED SEQUENCE TO BOB THROUGH QUANTUM CHANNEL
	double** en_bob_str;
	int* bob_str;
	char bob[N];
	bob_str = (int*)malloc(N*sizeof(int*));
	en_bob_str=(double**)malloc(N*sizeof(double*));
	double** en_eve_str;
	int* eve_str;
	eve_str = (int*)malloc(N*sizeof(int*));
	en_eve_str=(double**)malloc(N*sizeof(double*));
	for(i=0;i<N;i++)
	{
		en_bob_str[i]=(double*)malloc(2*sizeof(double));
		en_eve_str[i]=(double*)malloc(2*sizeof(double));
	}
	Quantum_channel(en_alice_str,en_bob_str,en_eve_str,bob_str,eve_str,bob,N);


	//ALICE TELLS BOB THE SEQUENCE IN WHICH ENCODING WAS DONE THROUGH ANY CLASSICAL CHANNEL. THEY REMOVE THE ELEMENTS MEASURED IN WRONG DIRECTION AND FORM THE RAW KEY
	int* alice_rawkey1;
	int* bob_rawkey1;
	int j=0,key_size=0;
	alice_rawkey1=(int*)malloc(sizeof(int));
	bob_rawkey1=(int*)malloc(sizeof(int));
	for(i=0;i<N;i++)
	{
		printf("%c   %c\n",alice[i],bob[i]);
		if(alice[i]==bob[i])
		{
			alice_rawkey1[j]=alice_str[i];
			bob_rawkey1[j]=bob_str[i];
			printf("%d  %d\n",alice_rawkey1[j],bob_rawkey1[j]);
			j++;
			key_size++;
			alice_rawkey1=realloc(alice_rawkey1,(j+1)*sizeof(int));
			bob_rawkey1=realloc(bob_rawkey1,(j+1)*sizeof(int));
		}
	}


	//ALICE AND BOB SHARE A PART OF THE RAW KEY THROUGH THE CLASSICAL CHANNEL AND VERIFY EACH BIT FROM THAT PART TO GET THE AVERAGE ERROR. THE PART OF THE RAW KEY EXCHANGED IS THEN REMOVED FORM THE RAW KEY TO ENHANCE SECURITY
	printf("key_size==%d\n",key_size);
	int* alice_rawkey2;
	int* bob_rawkey2;
	alice_rawkey2=(int*)malloc(sizeof(int));
	bob_rawkey2=(int*)malloc(sizeof(int));
	int r=0;
	temp= key_size;
	double error;
	if(rand()%2==1)
	{
		for(i=0;i<temp/10;i++)
		{
			if(alice_rawkey1[i]!=bob_rawkey1[i])
			{
				r++;
			}
			key_size--;
		}
		for(i=temp/10;i<temp;i++)
		{
			alice_rawkey2[i-temp/10]=alice_rawkey1[i];
			bob_rawkey2[i-temp/10]=bob_rawkey1[i];
		}
	}
	else
	{
		for(i=(temp*9)/10;i<temp;i++)
		{
			if(alice_rawkey1[i]!=bob_rawkey1[i])
			{
				r++;
			}
			key_size--;
		}
		for(i=0;i<(temp*9)/10;i++)
		{
			alice_rawkey2[i]=alice_rawkey1[i];
			bob_rawkey2[i]=bob_rawkey1[i];
		}
	}
	error=(double)r*10.0/(double)temp;
	printf("r=no.of unequal bits=%d  error=%lf\n",r,error);
	printf("key_size=%d\n",key_size);


	//THE REMAINING RAW KEY IS NOW DIVIDED INTO MANY BLOCKS EACH OF SIZE l AND THEN PARITIES OF EACH BLOCK IS CALCULATED
	temp = 1/error;
	printf("1/error =%d\n",temp);
	i=j=k=0;
	int l,n;
	l=10;
	n=key_size/l;
	printf("no. of blocks=%d\n",n);
	int alice_block[n][l];
	int bob_block[n][l];
	for(i=0;i<n*l;i++)
	{
		if(i%l==0 && i!=0)
		{
			k++;
			j=0;
		}
		alice_block[k][j]=alice_rawkey2[i];
		bob_block[k][j]=bob_rawkey2[i];
		j++;
	}
	int alice_parity[n];
	int bob_parity[n];
	for(i=0;i<n;i++)
	{
		alice_parity[i]=Parity(alice_block[i],l);
		bob_parity[i]=Parity(bob_block[i],l);
	}


	//THE PARITIES OF CORRESPONDING BLOCKS ARE COMPARED, BLOCKS WITH SAME PARITY ARE ADDED TO THE FINAL KEY. BLOCKS WITH DIFFERENT PARITY ARE DIVIDED AGAIN AND COMPARED TILL WE FIND THE ERROR BITS WHICH ARE THEN REMOVED
	j=0;
	final.count=0;
	for(i=0;i<n;i++)
	{
		if(alice_parity[i]==bob_parity[i])
		{
			for(k=0;k<l;k++)
			{
				final.key[final.count]=alice_block[i][k];
				final.count++;
			}
		}
		else
		{
			paritycheck(alice_block[i],bob_block[i],final,l);
		}
	}
	printf("length of final key= %d\n",final.count);
	printf("FINAL KEY IS,\n");
	for(i=0;i<final.count;i++)
		printf("%d",final.key[i]);
	printf("\n");
}


void encode_X_dir(int* alice_str,double** en_alice_str, int i)
{
	if(alice_str[i]==1)
	{
		en_alice_str[i][0]=0.707;
		en_alice_str[i][1]=-0.707;
	}
	else if(alice_str[i]==0)
	{
		en_alice_str[i][0]=0.707;
		en_alice_str[i][1]=0.707;
	}
}

void encode_Z_dir(int* alice_str, double** en_alice_str, int i)
{
	if(alice_str[i]==1)
	{
		en_alice_str[i][0]=0;
		en_alice_str[i][1]=1;
	}
	else if(alice_str[i]==0)
	{
		en_alice_str[i][0]=1;
		en_alice_str[i][1]=0;
	}
}

void Quantum_channel(double** en_alice_str,double** en_bob_str,double** en_eve_str,int* bob_str,int* eve_str,char* bob,int N)
{
	int i,temp,temp2;
	puts("Eavesdrop (Adversary): Enter 1 or 0");
	scanf("%d",&temp);
	if(temp==1)
	{
		for(i=0;i<N;i++)
		{
			en_eve_str[i][0]=en_alice_str[i][0];
			en_eve_str[i][1]=en_alice_str[i][1];
		}
		for(i=0;i<N;i++)
		{
			if(rand()%2==0)
			{
				measure_X_dir(en_eve_str,eve_str,i);
			}
			else
			{
				measure_Z_dir(en_eve_str,eve_str,i);
			}
			printf("eve %lf %lf     %d\n",en_eve_str[i][0],en_eve_str[i][1],eve_str[i]);
		}
		for(i=0;i<N;i++)
		{
			en_bob_str[i][0]=en_eve_str[i][0];
			en_bob_str[i][1]=en_eve_str[i][1];
		}
		for(i=0;i<N;i++)
		{
			if(rand()%2==0)
			{
				measure_X_dir(en_bob_str,bob_str,i);
				bob[i]='x';
			}
			else
			{
				measure_Z_dir(en_bob_str,bob_str,i);
				bob[i]='z';
			}
			printf("bob %lf %lf     %d\n",en_bob_str[i][0],en_bob_str[i][1],bob_str[i]);
		}
	}
	else
	{
		for(i=0;i<N;i++)
		{
			en_bob_str[i][0]=en_alice_str[i][0];
			en_bob_str[i][1]=en_alice_str[i][1];
		}
		for(i=0;i<N;i++)
		{
			if(rand()%2==0)
			{
				measure_X_dir(en_bob_str,bob_str,i);
				bob[i]='x';
			}
			else
			{
				measure_Z_dir(en_bob_str,bob_str,i);
				bob[i]='z';
			}
			printf("bob %lf %lf     %d\n",en_bob_str[i][0],en_bob_str[i][1],bob_str[i]);
		}
	}
	printf("Noise, Transmission Loss: Enter 1 or 0\n");
	scanf("%d",&temp2);
	if(temp2==1)
	{
		for(i=0;i<N;i++)
		{
			if(rand()%7==0)
			{
				if(bob_str[i]==1)
					bob_str[i]=0;
				else 
					bob_str[i]=1;
			}
		}	
	}
}


void measure_X_dir(double** en_str,int* str,int i)
{
	if(en_str[i][1]==0.707 && en_str[i][0]==0.707)
	{
		str[i]=0;
	}
	else if(en_str[i][1]==-0.707 && en_str[i][0]==0.707)
	{
		str[i]=1;
	}
	else if(en_str[i][1]==0 || en_str[i][1]==1)
	{
		if(rand()%2==1)
		{
			str[i]=0;
			en_str[i][0]=0.707;
			en_str[i][1]=0.707;
		}
		else
		{
			str[i]=1;
			en_str[i][0]=0.707;
			en_str[i][1]=-0.707;
		}
	}
}

void measure_Z_dir(double** en_str,int* str,int i)
{
	
	if(en_str[i][1]==1)
	{
		str[i]=1;
	}
	else if(en_str[i][1]==0)
	{
		str[i]=0;
	}
	else if(en_str[i][1]==0.707 || en_str[i][1]==-0.707)
	{
		if(rand()%2==1)
		{
			str[i]=0;
			en_str[i][0]=1;
			en_str[i][1]=0;
		}
		else
		{
			str[i]=1;
			en_str[i][0]=0;
			en_str[i][1]=1;
		}
	}
}

int Parity(int* block,int l)
{
	int i,p=0;
	for(i=0;i<l;i++)
	{
		if(p==0 && block[i]==0)
			p+=block[i];
		else if(p==0 && block[i]==1)
			p+=block[i];
		else if(p==1 && block[i]==0)
			p+=block[i];
		else if(p==1 && block[i]==1)
			p=0;
	}
	return p;
}

void paritycheck(int* alice_block,int* bob_block,struct stack final,int l)
{
	int i,j;
	int aliceB1[l/2], aliceB2[l/2];
	int bobB1[l/2], bobB2[l/2];
	for(i=0;i<l/2;i++)
	{
		aliceB1[i]=alice_block[i];
		bobB1[i]=bob_block[i];
	}
	for(i=l/2;i<l;i++)
	{
		aliceB2[i-l/2]=alice_block[i];
		bobB2[i-l/2]=bob_block[i];
	}
	if(Parity(aliceB1,l/2)==Parity(bobB1,l/2))
	{
		for(j=0;j<l/2;j++)
		{
			final.key[final.count]=aliceB1[j];
			final.count++;
		}
	}
	else 
	{
		if(l/2!=1)
			paritycheck(aliceB1,bobB1,final,l/2);
	}
	if(Parity(aliceB2,l/2)==Parity(bobB2,l/2))
	{
		for(j=0;j<l/2;j++)
		{
			final.key[final.count]=aliceB2[j];
			final.count++;
		}
	}
	else 
	{
		if(l/2!=1)
			paritycheck(aliceB2,bobB2,final,l/2);
	}
}