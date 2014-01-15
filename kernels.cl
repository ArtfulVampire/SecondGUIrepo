
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int64_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int64_extended_atomics : enable

#pragma OPENCL EXTENSION cl_khr_global_float32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_float32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_float64_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_float64_extended_atomics : enable

#pragma OPENCL EXTENSION cl_khr_global_double32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_double32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_double64_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_double64_extended_atomics : enable

__kernel void leaveOneOut(
__global double ecrit,
__global double lrate,
__global double error,
__global double temp,
__global double ** matrix,
__global int NumberOfVectors,
__global int NumOfClasses,
__global int NetLength,
__private double ** weight,
__private int * mixNum,
__private double * output,
__private bool answer,
__private double outError,
__private double * outputClass,
__global double * NumberOfErrors,
__private int NumOfThread
)
{
    int vecNum;
    double currentError = 2.*ecrit;
    int type=0;

    //set zero weights
    for(int i=0; i<NetLength+1; ++i)
    {
        for(int j=0; j<NumOfClasses; ++j)
        {
            weight[j][i]=0.;
        }
    }

    for(int i=0; i<NumberOfVectors; ++i)
    {
        mixNum[i]=i;
    }

    //part from tall();
    Error=0.;
    NumberOfErrors = new int[NumOfClasses];
    helpString="";
    for(int i=0; i<NumOfClasses; ++i)
    {
        NumberOfErrors[i]=0;
    }


    int a1, a2, buffer;

        NumOfThread = get_global_id(0)

        epoch=0;

        while(currentError>ecrit)
        {
            currentError=0.0;
            //mix vectors
            for(int i=0; i<5*NumberOfVectors; ++i)
            {
                a1=rand()%(NumberOfVectors);
                a2=rand()%(NumberOfVectors);
                buffer=mixNum[a2];
                mixNum[a2]=mixNum[a1];
                mixNum[a1]=buffer;
            }
            //        cout<<"epoch="<<epoch<<endl;

            for(vecNum=0; vecNum<NumberOfVectors; ++vecNum)
            {
                if(mixNum[vecNum] == NumOfThread) continue; //not to learn with h'th vector

                type=int(matrix[mixNum[vecNum]][NetLength+1]);
                //            cout<<"type="<<type<<endl;

                for(int j=0; j<NumOfClasses; ++j) //calculate output
                {
                    output[j]=0.;
                    for(int i=0; i<NetLength+1; ++i)   // +bias, coz +1
                    {
                        output[j]+=weight[j][i]*matrix[mixNum[vecNum]][i];
                    }
                    output[j]=logistic(output[j], temp); // unlinear logistic conformation
                }

                //            cout<<"output[0]="<<output[0]<<"  output[1]="<<output[1]<<endl;

                //error count + weight differ
                currentError+=(1.-output[type])*(1.-output[type]);
                for(int i=0; i<NumOfClasses; ++i)
                {
                    if(i!=type)
                    {
                        currentError+=output[i]*output[i];
                    }
                }

                int i=0;
                while(i<(NetLength+1))
                {
                    //cbypass of channels not to consider
                    for(int z=0; z<zeroChanLength; ++z)
                    {
                        if((i/spLength+1)==zeroChan[z]) i+=spLength;
                    }

                    weight[type][i] += lrate * (1.-output[type]) * matrix[mixNum[vecNum]][i];
                    for(int k=0; k<NumOfClasses; ++k)
                    {
                        if (k!=type) weight[k][i] -= lrate * output[k] * matrix[mixNum[vecNum]][i];
                    }
                    ++i;
                }
            }

            currentError/=NumberOfVectors;
            currentError=sqrt(currentError);
            ++epoch;
        }

int type = int(matrix[NumOfThread][NetLength+1]);

for(int j = 0; j < NumOfClasses; ++j) //calculate output //2 = numberOfTypes
{
    outputClass[j] = 0.;
    for(int i = 0; i < NetLength; ++i)
    {
        outputClass[j]+=weight[j][i] * matrix[NumOfThread][i];
    }
    outputClass[j] += weight[j][NetLength] * matrix[NumOfThread][NetLength];
    outputClass[j] = logistic(outputClass[j], temp); // unlinear conformation
}
bool right = 1;
double outp = outputClass[type];
for(int k = 0; k < NumOfClasses; ++k)
{
    if(k != type && outputClass[k] >= outp)
    {
        right = false;
        outp = outputClass[k];
    }
}
if(!right && matrix[NumOfThread][NetLength+1]!=1.5) ++NumberOfErrors[type]; //generality
outError = 0.;
for(int k = 0; k < NumOfClasses; ++k)
{
    if(k!=type)
    {
        outError += (outputClass[k] * outputClass[k]);
    }
    else
    {
        outError += (1. - outputClass[k]) * (1. - outputClass[k]);
    }
}
outError = atom_sqrt(outError);
answer = right; //return value
}
