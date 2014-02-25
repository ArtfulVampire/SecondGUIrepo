#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#pragma OPENCL EXTENSION cl_amd_fp64 : enable

int rand(int randCounter)
{
return randArr[randCounter%1000];
}

double logistic(double x, double t)
{
    if( x >   37.*t )  return 1.;
    if( x < -115.*t )  return 0.;
    return 1. / ( 1. + exp(-x/t) );
}

__kernel void leaveOneOut(
 double ecrit,
 double lrate,
 double temp,
global double * matrix, //NumberOfVectors * (NetLength+2)
 int NumberOfVectors,
 int NumOfClasses,
 int NetLength,
local double * weight, //NumberOfClasses * (NetLength+1)
local int * mixNum,
local double * output,
global bool * answer,
global double * outError,
local double * outputClass,
global double * NumberOfErrors,
global int * NumOfThread,
global int * NumOfVectorToSkip)
{
    int vecNum;
    double currentError = 2. * ecrit;
    int type=0;
    int randCounter = 12*get_global_id(0);

    NumOfThread[get_global_id(0)] = get_global_id(0);

    NumOfVectorToSkip[get_global_id(0)] = get_global_id(0);

    for(int i=0; i<NetLength+1; ++i)
    {
        for(int j=0; j<NumOfClasses; ++j)
        {
            weight[j * (NetLength+1) + i]=0.;
        }
    }

    for(int i=0; i<NumberOfVectors; ++i)
    {
        mixNum[i]=i;
    }



    //NumberOfErrors = new int[NumOfClasses];
    //helpString="";
    for(int i = 0; i < NumOfClasses; ++i)
    {
        NumberOfErrors[i] = 0;
    }


    int a1, a2, buffer;

        int epoch=0;

        while(currentError>ecrit)
        {
            currentError=0.0;
            //mix vectors
            for(int i=0; i<5*NumberOfVectors; ++i)
            {
                a1 = rand(randCounter++)%(NumberOfVectors);
                a2 = rand(randCounter++)%(NumberOfVectors);
                buffer=mixNum[a2];
                mixNum[a2]=mixNum[a1];
                mixNum[a1]=buffer;
            }
            //        cout<<"epoch="<<epoch<<endl;

            for(vecNum = 0; vecNum < NumberOfVectors; ++vecNum)
            {
                if( mixNum[vecNum] == NumOfVectorToSkip[get_global_id(0)] ) continue; //not to learn with h'th vector

                type = matrix[mixNum[vecNum] * (NetLength+2) + NetLength+1];

                for(int j = 0; j<NumOfClasses; ++j) //calculate output
                {
                    output[j]=0.;
                    for(int i=0; i<NetLength+1; ++i)   // +bias, coz +1
                    {
                        output[j]+=weight[j * (NetLength+1) + i]*matrix[mixNum[vecNum] * (NetLength+2) + i];
                    }
                    output[j] = logistic(output[j], temp); // unlinear logistic conformation
//                    output[j] = 1. / ( 1. + exp(-output[j] / temp) );
                }

                //error count + weight differ
                currentError+=(1.-output[type])*(1.-output[type]);
                for(int i=0; i<NumOfClasses; ++i)
                {
                    if(i!=type)
                    {
                        currentError += output[i] * output[i];
                    }
                }

//                int i=0;
//                while(i<(NetLength+1))
//                {
//                    //cbypass of channels not to consider
//                    for(int z=0; z<zeroChanLength; ++z)
//                    {
//                        if((i/spLength+1)==zeroChan[z]) i+=spLength;
//                    }

//                    weight[type][i] += lrate * (1.-output[type]) * matrix[mixNum[vecNum]][i];
//                    for(int k=0; k<NumOfClasses; ++k)
//                    {
//                        if (k!=type) weight[k][i] -= lrate * output[k] * matrix[mixNum[vecNum]][i];
//                    }
//                    ++i;
//                }

                //learn itself
                for(int i = 0; i < (NetLength+1); ++i)
                {
                weight[type * (NetLength+1) + i] += lrate * (1.-output[type]) * matrix[mixNum[vecNum] * (NetLength+2) + i];
                for(int k=0; k<NumOfClasses; ++k)
                {
                    if (k!=type) weight[k * (NetLength+1) + i] -= lrate * output[k] * matrix[mixNum[vecNum] * (NetLength+2) + i];
                }
                }
            }

            currentError/=NumberOfVectors;
            currentError=sqrt(currentError);
            ++epoch;
        }



type = matrix[NumOfVectorToSkip[get_global_id(0)] * (NetLength+2) + NetLength+1];
for(int j = 0; j < NumOfClasses; ++j) //calculate output //2 = numberOfTypes
{
    outputClass[j] = 0.;
    for(int i = 0; i < NetLength; ++i)
    {
        outputClass[j]+=weight[j * (NetLength+1) + i] * matrix[NumOfVectorToSkip[get_global_id(0)] * (NetLength+2) + i];
    }
    outputClass[j] += weight[j * (NetLength+1) + NetLength] * matrix[NumOfVectorToSkip[get_global_id(0)] * (NetLength+2) + NetLength];
    outputClass[j] = logistic(outputClass[j], temp); // unlinear conformation
//    outputClass[j] = 1. / ( 1. + exp(-outputClass[j] / temp) );
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
if(!right && matrix[NumOfVectorToSkip[get_global_id(0)] * (NetLength+2) + NetLength+1]!=1.5) ++NumberOfErrors[type]; //generality
outError[NumOfVectorToSkip[get_global_id(0)]] = 0.;
for(int k = 0; k < NumOfClasses; ++k)
{
    if(k!=type)
    {
        outError[NumOfVectorToSkip[get_global_id(0)]] += (outputClass[k] * outputClass[k]);
    }
    else
    {
        outError[NumOfVectorToSkip[get_global_id(0)]] += (1. - outputClass[k]) * (1. - outputClass[k]);
    }
}
outError[NumOfVectorToSkip[get_global_id(0)]] = sqrt(outError[NumOfVectorToSkip[get_global_id(0)]]);
answer[NumOfVectorToSkip[get_global_id(0)]] = right; //return value
}



