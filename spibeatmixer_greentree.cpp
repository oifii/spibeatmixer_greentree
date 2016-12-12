/*
 * Copyright (c) 2010-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

////////////////////////////////////////////////////////////////
//nakedsoftware.org, spi@oifii.org or stephane.poirier@oifii.org
//
//
//2013nov13, creation for constructing rhythm
//
//
//nakedsoftware.org, spi@oifii.org or stephane.poirier@oifii.org
////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <string>
#include <fstream>
#include <vector>

#include <iostream>
#include <sstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"

#define BUFF_SIZE	2048


#include <ctime>
#include "spiws_WavSet.h"
#include "spiws_Instrument.h"
#include "spiws_InstrumentSet.h"

#include "spiws_partition.h"
#include "spiws_partitionset.h"

#include "spiws_WavSet.h"

#include <assert.h>
#include <windows.h>



// Select sample format
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

//The event signaled when the app should be terminated.
HANDLE g_hTerminateEvent = NULL;
//Handles events that would normally terminate a console application. 
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);

int Terminate();

InstrumentSet* pInstrumentSet=NULL;
InstrumentSet* pInstrumentSet2=NULL;
InstrumentSet* pInstrumentSet3=NULL;


//////////////////////////////////////////
//main
//////////////////////////////////////////
int main(int argc, char *argv[]);
int main(int argc, char *argv[])
{
    //Auto-reset, initially non-signaled event 
    g_hTerminateEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    //Add the break handler
    ::SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

	PaStreamParameters outputParameters;
    PaStream* stream;
    PaError err;

	////////////////////////
	// initialize port audio 
	////////////////////////
    err = Pa_Initialize();
    if( err != paNoError )
	{
		fprintf(stderr,"Error: Initialization failed.\n");
		Pa_Terminate();
		fprintf( stderr, "An error occured while using the portaudio stream\n" );
		fprintf( stderr, "Error number: %d\n", err );
		fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
		return -1;
	}

	outputParameters.device = Pa_GetDefaultOutputDevice(); // default output device 
	if (outputParameters.device == paNoDevice) 
	{
		fprintf(stderr,"Error: No default output device.\n");
		Pa_Terminate();
		fprintf( stderr, "An error occured while using the portaudio stream\n" );
		fprintf( stderr, "Error number: %d\n", err );
		fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
		return -1;
	}
	outputParameters.channelCount = 2;//pWavSet->numChannels;
	outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;


	//////////////////////////
	//initialize random number
	//////////////////////////
	srand((unsigned)time(0));



	/////////////////////////////
	//loop n sinusoidal samples
	/////////////////////////////
	WavSet* pTempWavSet = new WavSet;
	pTempWavSet->CreateSin(0.3333, 44100, 2, 440.0, 0.5f);
	WavSet* pSilentWavSet = new WavSet;
	pSilentWavSet->CreateSilence(5);
	pSilentWavSet->LoopSample(pTempWavSet, 5, -1.0, 0.0); //from second 0, loop sample during 5 seconds
	pSilentWavSet->Play(&outputParameters);
	if(pTempWavSet)
	{
		delete pTempWavSet;
		pTempWavSet = NULL;
	}
	if(pSilentWavSet)
	{
		delete pSilentWavSet;
		pSilentWavSet = NULL;
	}
	

	

	//////////////////////////////////////////////////////////////////////////
	//populate InstrumentSet according to input folder (folder of sound files)
	//////////////////////////////////////////////////////////////////////////
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\raveaudio_wav\\dj-oifii_minimal-deep-electro-house-techno";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\raveaudio_wav\\dj-oifii_gouryella";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\raveaudio_wav\\dj-oifii_ibiza";

	string wavfolder = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\worldaudio_wav\\00min30sec-and-less\\Hotel Costes 1 - 04 Green Tree(introlater)_20sec";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\worldaudio_wav\\00min15sec-and-less";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\worldaudio_wav\\00min30sec-and-less";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\worldaudio_wav\\01min00sec-and-less";

	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test001(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test002(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test003(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test004(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test005(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test006(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test007(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test008(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test009(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test010(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test011(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test012(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test013(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test014(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test015(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test016(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test017(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test018(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test019(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test020(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test021(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test022(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test023(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test024(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test025(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test026(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test027(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test028(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test029(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test030(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test031(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test032(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test033(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test034(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test035(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test036(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test037(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test038(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test039(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test040(2013avril21)";
	//string wavfolder = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave\\recycle_test041(2013avril21)";


	//string wavfolder2 = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\bookaudio_wav\\audio_audio-books_ekhart-tolle_the-power-of-now_chapter1-track-no01";
	string wavfolder2 = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\bookaudio_wav\\audio_audio-books_ekhart-tolle_the-power-of-now_chapter1-track-no01_slices";
	//string wavfolder2 = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\raveaudio_wav\\dj-oifii_minimal-deep-electro-house-techno\\dj-oifii_minimal-deep-electro-house-techno(beat-offset-8min08sec)";
	//string wavfolder2 = "D:\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_wave";

	string wavfolder3 = "D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\worldaudio_wav\\00min30sec-and-less\\Hotel Costes 1 - 04 Green Tree(introlater)_20sec\\bass";

	/*
	InstrumentSet* pInstrumentSet=new InstrumentSet;
	InstrumentSet* pInstrumentSet2=new InstrumentSet;
	*/
	pInstrumentSet=new InstrumentSet;
	pInstrumentSet2=new InstrumentSet;
	pInstrumentSet3=new InstrumentSet;
	//InstrumentSet* pInstrumentSet3=new InstrumentSet;
	//if(pInstrumentSet!=NULL && pInstrumentSet2!=NULL && pInstrumentSet3!=NULL)
	if(pInstrumentSet!=NULL && pInstrumentSet2!=NULL)
	{
		pInstrumentSet->Populate(wavfolder.c_str(), 0);
		pInstrumentSet2->Populate(wavfolder2.c_str(), 0);
		pInstrumentSet3->Populate(wavfolder3.c_str(), 0);
	}
	else
	{
		assert(false);
		cout << "exiting, instrumentset could not be allocated" << endl;
		Pa_Terminate();
		return -1;
	}
		

	/*
	///////////////////////////////////
	//play all notes of all instruments
	///////////////////////////////////
	//int numberofinstrumentsinplayback=3;
	int numberofinstrumentsinplayback=1; //one instrument at a time
	float fSecondsPlay = 600.0f;
	int iCONCATENATEATTACKSflag = 0; //0 false, 1 true
	pInstrumentSet->Play(&outputParameters, fSecondsPlay, numberofinstrumentsinplayback, iCONCATENATEATTACKSflag); //each instrument will play its loaded samples sequentially
	
	int dummy=0;
	*/

	/*
	//////////////////////////////////////////////////////////////////////////////////////////////
	//pick random instrument, pick random wavset, loop wavset for loopduration_s seconds and play,
	//then repeat all of the above 100 times
	//////////////////////////////////////////////////////////////////////////////////////////////
	Instrument* pAnInstrument = NULL;
	WavSet* pAWavSet = NULL;
	for(int i=0; i<100; i++) //repeat 100 times
	{
		if(pInstrumentSet && pInstrumentSet->HasOneInstrument()) 
		{
			/////////////
			//pick sample
			/////////////
			cout << endl;
			//pAnInstrument = *it;
			pAnInstrument = pInstrumentSet->GetInstrumentRandomly();
			assert(pAnInstrument);
			cout << "instrument name: " << pAnInstrument->instrumentname << endl;
			pAWavSet = pAnInstrument->GetWavSetRandomly(); 
			//while (pAWavSet->SampleRate==0) { pAWavSet = pAnInstrument->GetWavSetRandomly(); }
			cout << "sound filename: " << pAWavSet->GetName() << endl;
			assert(pAWavSet);
			//pAWavSet->Play(&outputParameters);

			////////////////
			//add some space
			////////////////
			WavSet* pSilentWavSet1 = new WavSet;
			float silenceduration_s = 3 * pAWavSet->GetWavSetLength() + 0.050f; //0.050 sec to ensure loopduration_s is larger than sample
			pSilentWavSet1->CreateSilence(silenceduration_s); 
			pSilentWavSet1->Sum(1.0, pAWavSet, 0.0, silenceduration_s); 

			//////////////////
			//loop a few times
			//////////////////
			float loopduration_s = 5 * pSilentWavSet1->GetWavSetLength() + 0.050f; //0.050 sec to ensure loopduration_s is larger than sample
			//WavSet* pSilentWavSet = new WavSet;
			pSilentWavSet = new WavSet;
			pSilentWavSet->CreateSilence(loopduration_s); 
			pSilentWavSet->LoopSample(pSilentWavSet1, loopduration_s, -1.0, 0.0); //from second 0, loop sample during loopduration_s seconds
			//pSilentWavSet->Play(&outputParameters);

			/////////////////////////////////////////////////////////////////
			//spread many smaller-scale samples, in this case, ekhart's voice
			/////////////////////////////////////////////////////////////////
			Instrument* pAnInstrument2 = pInstrumentSet2->GetInstrumentRandomly();
			if(pAnInstrument2)
			{
				//WavSet* pAWavSet2 = pAnInstrument2->GetWavSetRandomly();
				//if(pAWavSet2)
				//{
					//pSilentWavSet->SpreadSample("1111110011101100", pAWavSet2, loopduration_s, 0.5, 0.0); //pattern over loopduration_s sec, sample duration 0.5sec and with pattern offset of 0sec;// pAWavSet2
					//pSilentWavSet->SpreadSamples("1101110011101100", pAnInstrument2, loopduration_s, 5, 0.0); //pattern over loopduration_s sec, sample duration 5sec and with pattern offset of 0sec;// pAWavSet2
					pSilentWavSet->SpreadSamples("1000000000000000", pAnInstrument2, loopduration_s, 10, 0.0); //pattern over loopduration_s sec, sample duration 5sec and with pattern offset of 0sec;// pAWavSet2
				//}
			}

				
			////////////
			//play
			////////////
			pSilentWavSet->Play(&outputParameters);
				
			//pSilentWavSet->Erase();
			//pSilentWavSet->Play(&outputParameters);
			if(pSilentWavSet)
			{
				delete pSilentWavSet;
				pSilentWavSet = NULL;
			}
			if(pSilentWavSet1)
			{
				delete pSilentWavSet1;
				pSilentWavSet1 = NULL;
			}
		}
	}
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////
	//pick random instrument, pick random wavset, loop wavset for loopduration_s seconds and play,
	//then repeat all of the above 100 times
	//////////////////////////////////////////////////////////////////////////////////////////////
	Instrument* pAnInstrument = NULL;
	WavSet* pAWavSet[20];
	for(int i=0; i<100; i++) //repeat 100 times
	{
		if(pInstrumentSet && pInstrumentSet->HasOneInstrument()) 
		{
			/*
			WavSet* pSilentWavSet1 = new WavSet;
			for(int ii=0; ii<20; ii++)
			{
				//////////////
				//pick samples
				//////////////
				cout << endl;
				//pAnInstrument = *it;
				pAnInstrument = pInstrumentSet->GetInstrumentRandomly();
				assert(pAnInstrument);
				cout << "instrument name: " << pAnInstrument->instrumentname << endl;
				pAWavSet[ii] = pAnInstrument->GetWavSetRandomly(); 
				//while (pAWavSet->SampleRate==0) { pAWavSet = pAnInstrument->GetWavSetRandomly(); }
				cout << "sound filename: " << pAWavSet[ii]->GetName() << endl;
				assert(pAWavSet[ii]);
				//pAWavSet[ii]->Play(&outputParameters);
				if(ii==0)
				{
					//copy
					pSilentWavSet1->Copy(pAWavSet[ii]);
				}
				else
				{
					//concatenate
					pSilentWavSet1->Concatenate(pAWavSet[ii]);
				}
			}
			*/
			WavSet* pSilentWavSet1 = new WavSet;
			for(int ii=0; ii<pInstrumentSet->GetNumberOfInstrument(); ii++)
			{
				pAnInstrument = pInstrumentSet->GetInstrumentFromID(ii);
				assert(pAnInstrument);
				cout << "instrument name: " << pAnInstrument->instrumentname << endl;
				for(int jj=0; jj<pAnInstrument->GetNumberOfWavSet(); jj++)
				{
					pAWavSet[0] = pAnInstrument->GetWavSetFromID(jj); 
					assert(pAWavSet[0]);
					cout << "sound filename: " << pAWavSet[0]->GetName() << endl;
					if(ii==0 && jj==0)
					{
						//copy
						pSilentWavSet1->Copy(pAWavSet[0]);
					}
					else
					{
						//concatenate
						pSilentWavSet1->Concatenate(pAWavSet[0]);
					}
				}
			}
			//////////////////
			//loop a few times
			//////////////////
			float loopduration_s = 1 * pSilentWavSet1->GetWavSetLength() + 0.050f; //0.050 sec to ensure loopduration_s is larger than sample
			//WavSet* pSilentWavSet = new WavSet;
			pSilentWavSet = new WavSet;
			pSilentWavSet->CreateSilence(loopduration_s); 
			pSilentWavSet->LoopSample(pSilentWavSet1, loopduration_s, -1.0, 0.0); //from second 0, loop sample during loopduration_s seconds
			//pSilentWavSet->Play(&outputParameters);

			/*
			/////////////////////////////////////////////////////////////////
			//spread many smaller-scale samples, in this case, ekhart's voice
			/////////////////////////////////////////////////////////////////
			Instrument* pAnInstrument2 = pInstrumentSet2->GetInstrumentRandomly();
			if(pAnInstrument2)
			{
				//WavSet* pAWavSet2 = pAnInstrument2->GetWavSetRandomly();
				//if(pAWavSet2)
				//{
					//pSilentWavSet->SpreadSample("1111110011101100", pAWavSet2, loopduration_s, 0.5, 0.0); //pattern over loopduration_s sec, sample duration 0.5sec and with pattern offset of 0sec;// pAWavSet2
					//pSilentWavSet->SpreadSamples("1101110011101100", pAnInstrument2, loopduration_s, 5, 0.0); //pattern over loopduration_s sec, sample duration 5sec and with pattern offset of 0sec;// pAWavSet2
					pSilentWavSet->SpreadSamples("1000000000000000", pAnInstrument2, loopduration_s, 10, 0.0); //pattern over loopduration_s sec, sample duration 5sec and with pattern offset of 0sec;// pAWavSet2
				//}
			}
			*/
				
			////////////
			//play
			////////////
			pSilentWavSet->Play(&outputParameters);
				
			//pSilentWavSet->Erase();
			//pSilentWavSet->Play(&outputParameters);
			if(pSilentWavSet)
			{
				delete pSilentWavSet;
				pSilentWavSet = NULL;
			}
			if(pSilentWavSet1)
			{
				delete pSilentWavSet1;
				pSilentWavSet1 = NULL;
			}
		}

		////////////
		//other demo
		////////////
		WavSet* pTickWavSet = new WavSet;
		pTickWavSet->ReadWavFile("D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\worldaudio_wav\\00min30sec-and-less\\Hotel Costes 1 - 04 Green Tree(introlater)_20sec\\begining_004\\tick.wav");
		WavSet* pBackgroundWavSet = new WavSet;
		pBackgroundWavSet->ReadWavFile("D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\worldaudio_wav\\00min30sec-and-less\\Hotel Costes 1 - 04 Green Tree(introlater)_20sec\\begining_004\\background.wav");
		WavSet* pBassWavSet = new WavSet;
		pBassWavSet->ReadWavFile("D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\worldaudio_wav\\00min30sec-and-less\\Hotel Costes 1 - 04 Green Tree(introlater)_20sec\\begining_006.wav");

		WavSet* pSilentWavSet10 = new WavSet;
		while(pSilentWavSet10->GetWavSetLength()<10.0)
		{
			if(pSilentWavSet10->GetWavSetLength()==0.0)
			{
				//copy
				pSilentWavSet10->Copy(pBackgroundWavSet);
			}
			else
			{
				//concatenate
				pSilentWavSet10->Concatenate(pBackgroundWavSet);
			}
		}
		//play
		//pSilentWavSet10->Play(&outputParameters);


		WavSet* pSilentWavSet11 = new WavSet;
		pSilentWavSet11->CreateSilence(pSilentWavSet10->GetWavSetLength());
		pSilentWavSet11->Sum(0.2, pSilentWavSet10, 0.0, pSilentWavSet10->GetWavSetLength());
		//play
		//pSilentWavSet11->Play(&outputParameters);

		pSilentWavSet11->SpreadSample("1000100010001000100010001000100010001000", pTickWavSet, pSilentWavSet10->GetWavSetLength(), pTickWavSet->GetWavSetLength(), 0.0); 
		pSilentWavSet11->SpreadSample("0011001100110011001100110011001100110011", pBassWavSet, pSilentWavSet10->GetWavSetLength(), pBassWavSet->GetWavSetLength(), 0.0); 
		Instrument* pAnInstrument3 = pInstrumentSet3->GetInstrumentRandomly();
		if(pAnInstrument3)
		{
			pSilentWavSet11->SpreadSamples("0100010001000100010001000100010001000100", pAnInstrument3, pSilentWavSet10->GetWavSetLength(), 2.0, 0.0); 
		}
		//play
		pSilentWavSet11->Play(&outputParameters);

		if(pTickWavSet) delete pTickWavSet;
		if(pBackgroundWavSet) delete pBackgroundWavSet;
		if(pBassWavSet) delete pBassWavSet;
		if(pSilentWavSet10) delete pSilentWavSet10;
		if(pSilentWavSet11) delete pSilentWavSet11;

	}

	/*
	/////////////////////
	//terminate portaudio
	/////////////////////
	Pa_Terminate();
	//if(pInstrumentSet) delete pInstrumentSet;
	printf("Exiting!\n"); fflush(stdout);
	*/
	Terminate();
	return 0;
}

int Terminate()
{
	/////////////////////
	//terminate portaudio
	/////////////////////
	Pa_Terminate();

	//delete all memory allocations
	if(pInstrumentSet!=NULL) delete pInstrumentSet;
	if(pInstrumentSet2!=NULL) delete pInstrumentSet2;
	if(pInstrumentSet3!=NULL) delete pInstrumentSet3;

	printf("Exiting!\n"); fflush(stdout);
	return 0;
}

//Called by the operating system in a separate thread to handle an app-terminating event. 
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT ||
        dwCtrlType == CTRL_BREAK_EVENT ||
        dwCtrlType == CTRL_CLOSE_EVENT)
    {
        // CTRL_C_EVENT - Ctrl+C was pressed 
        // CTRL_BREAK_EVENT - Ctrl+Break was pressed 
        // CTRL_CLOSE_EVENT - Console window was closed 
		Terminate();
        // Tell the main thread to exit the app 
        ::SetEvent(g_hTerminateEvent);
        return TRUE;
    }

    //Not an event handled by this function.
    //The only events that should be able to
	//reach this line of code are events that
    //should only be sent to services. 
    return FALSE;
}

