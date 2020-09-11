# RPC FebConnectivity Test Analyzer

## Original Authors

**Developed by [Philip Thyssen](https://github.com/philipthyssen) and [Piet Verwilligen](https://github.com/pietverwilligen).**


## Setup (@lxplus)

Setup the environment:

```
cd /cvmfs/cms.cern.ch/slc6_amd64_gcc630/cms/cmssw/CMSSW_10_1_9/ 
cmsenv
cd -
```

Clone the source code:

```
git clone git@github.com:ftorresd/RPCFebConnectivityTest.git
```

## Compile:

Export $ROOTSYS. Depends on your system config. Could be that this is even already set.

```
export ROOTSYS=/usr/local/Cellar/root/6.18.04_1
```

```
cd RPCFebConnectivityTest
# ~/cmakeInstall/cmake-3.15.0-rc1/bin/cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_STANDARD=11 .
cmake .
make
```

## Docker

Launch the container.

```
docker run -v `pwd`:/opt/febconntest -it ftorresd/rpcfebconnectivitytest_buildenv
```

Inside the container.

```
cd /opt/febconntest
cmake .
```

## Running

From bare-metal build:

```
bin/connectivitytest -O output_filename.txt DB_filename.db
```

From Docker:

```
docker run -v `pwd`:/data ftorresd/rpcfebconnectivitytest connectivitytest -O output_filename.txt DB_filename.db
```


The .db file was generated by Trigger Supervisor ([documentation](https://twiki.cern.ch/twiki/bin/view/CMS/RPCFebCommissioningPanel)).


Three files will be created:
* ```output_filename.txt```: stdout of the analyzer, including diagnoses.
* ```DB_filename.pdf```: configurations used to create the .db and histograms with results.
* ```DB_filename.root```: same as the .pdf, but in root format.


