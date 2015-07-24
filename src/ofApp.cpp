#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofBackground(40, 51, 66);
	ofSetVerticalSync(true);

	//---------------------------

	//Sound
	_SoundPlayer.loadSound("test4.mp3");
	_SoundPlayer.setLoop(false);
	_SoundPlayer.setMultiPlay(false);
	_SoundPlayer.getVolume();
	ZeroMemory(_FFTBands, sizeof(float) * cNUM_OF_USED_BANDS);
	_fTotalVol = 0.0;

	//---------------------------
	_TotalLength = ofGetWidth() * 2 + ofGetHeight() * 2;
	_fBacketWidth = _TotalLength / (float)cNUM_OF_USED_BANDS;
	_AnimPos.setCurve(AnimCurve::LINEAR);
	_AnimPos.setRepeatType(AnimRepeat::LOOP);
	_AnimPos.setDuration(20);

	_StartPos[0] = _fBacketWidth/2;
	for(int idx_ = 1; idx_ < cNUM_OF_USED_BANDS; ++idx_)
	{
		_StartPos[idx_] = _StartPos[idx_ - 1] + _fBacketWidth;
	}

	_ArcVec1.set(_fBacketWidth/2, _fBacketWidth/2);
	_ArcVec2.set(ofGetWidth() - _fBacketWidth/2, _fBacketWidth/2);
	_ArcVec3.set(ofGetWidth() - _fBacketWidth/2, ofGetHeight() - _fBacketWidth/2);
	_ArcVec4.set(_fBacketWidth/2, ofGetHeight() - _fBacketWidth/2);
	
	_fGiraffeWidth = _fBacketWidth * 0.9;

	for(int idx_ = 0; idx_ < cNUM_OF_USED_BANDS; ++idx_)
	{
		_DyanmicGiraffe[idx_].setup(_fGiraffeWidth);

		_Direction[idx_] = (ofRandom(-1, 1) > 0.0);
	}
	
	//--------------------------------
	_JazzCat.setup();


	_fTimer = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::update()
{	
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
	float fDelta_ = ofGetElapsedTimef() - _fTimer;
	_fTimer += fDelta_;

	_JazzCat.update(fDelta_);

	if(!_SoundPlayer.getIsPlaying())
	{
		return;
	}

	_AnimPos.update(fDelta_);
	ofSoundUpdate();

	float* _fftResult = ofSoundGetSpectrum(cNUM_OF_FFT_BANDS);
	
	//Get volume avg.
	float fMaxValue_ = 0.0;
	float fNowVol = 0.0;
	for(int idx_ = 0; idx_ < cNUM_OF_USED_BANDS; ++idx_)
	{
		if(_fftResult[idx_] > fMaxValue_)
		{
			fMaxValue_ = _fftResult[idx_];
		}
		fNowVol += _fftResult[idx_];
	}
	fNowVol /= cNUM_OF_USED_BANDS;

	_fTotalVol *= 0.98;
	if(_fTotalVol < fNowVol)
	{
		_fTotalVol = fNowVol;
	}

	for(int idx_ = 0; idx_ < cNUM_OF_USED_BANDS; ++idx_)
	{
		_FFTBands[idx_] *= 0.98;
		if(_FFTBands[idx_] < _fftResult[idx_])
		{
			_FFTBands[idx_] = _fftResult[idx_];
			//_FFTBands[idx_] = _fftResult[idx_]/fMaxValue_;
		}

		_DyanmicGiraffe[idx_].update(_FFTBands[idx_] * 150);
	}

}

//--------------------------------------------------------------
void ofApp::draw()
{
	_JazzCat.draw();
	
	float fValue_ = _AnimPos.getCurrentValue();
	
	for(int idx_ = 0; idx_ < cNUM_OF_USED_BANDS; ++idx_)
	{
		ofVec2f Pos_ = this->getPos(_StartPos[idx_], fValue_);
		float fDegree_ = this->getRotate(Pos_);
		
		ofPushMatrix();
		ofTranslate(Pos_);
		ofRotateZ(fDegree_);
		ofTranslate(0, -_DyanmicGiraffe[idx_].getLength());
		{
			_DyanmicGiraffe[idx_].draw();
		}
		ofPopMatrix();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	switch(key)
	{
	case 'p':
		{
			if(_SoundPlayer.isLoaded())
			{
				_SoundPlayer.play();
				_AnimPos.animateFromTo(0.0, 1.0);
				_JazzCat.start();
			}
		}
		break;
	case 's':
		{
			if(_SoundPlayer.getIsPlaying())
			{
				_SoundPlayer.stop();
			}
		}
		break;
	}
}

//--------------------------------------------------------------
ofVec2f ofApp::getPos(float fStartLength, float fPerc)
{
	int iType_ = 0;
	float fLength_ = fStartLength + fPerc * _TotalLength;
	
	ofVec2f returnPos_(0);
	bool bLoop_(true);

	while(bLoop_)
	{
		iType_ = iType_ % 4;
		switch(iType_)
		{
		case 0:
			{
				if(fLength_ - ofGetWidth() <= 0.0)
				{
					returnPos_.x = fLength_;
					bLoop_ = false;					
				}
				fLength_ -= ofGetWidth();
			}
			break;
		case 1:
			{
				if(fLength_ - ofGetHeight() <= 0.0)
				{
					returnPos_.x = ofGetWidth();
					returnPos_.y = fLength_;
					bLoop_ = false;
					
				}
				fLength_ -= ofGetHeight();
			}
			break;
		case 2:
			{
				if(fLength_ - ofGetWidth() <= 0.0)
				{
					returnPos_.x = ofGetWidth() - fLength_;
					returnPos_.y = ofGetHeight();
					bLoop_ = false;

				}
				fLength_ -= ofGetWidth();
			}
			break;
		case 3:
			{
				if(fLength_ - ofGetHeight() <= 0.0)
				{
					returnPos_.y = ofGetHeight() - fLength_;
					bLoop_ = false;
				}
				fLength_ -= ofGetHeight();
			}
			break;
		}
		iType_++;
	}
	return returnPos_;

}

//--------------------------------------------------------------
float ofApp::getRotate(ofVec2f Pos)
{
	//Rotate
	if(Pos.x < _ArcVec1.x && Pos.y < _ArcVec1.y)
	{
		ofVec2f Vec_ = _ArcVec1 - Pos;
		return -Vec_.angle(ofVec2f(0, -1));
	}
	else if(Pos.x > _ArcVec2.x && Pos.y < _ArcVec2.y)
	{
		ofVec2f Vec_ = _ArcVec2 - Pos;
		return -Vec_.angle(ofVec2f(0, -1));
	}
	else if(Pos.x > _ArcVec3.x && Pos.y > _ArcVec3.y)
	{
		ofVec2f Vec_ = _ArcVec3 - Pos;
		return -Vec_.angle(ofVec2f(0, -1));
	}
	else if(Pos.x < _ArcVec4.x && Pos.y > _ArcVec4.y)
	{
		ofVec2f Vec_ = _ArcVec4 - Pos;
		return -Vec_.angle(ofVec2f(0, -1));
	}
	else
	{
		if(Pos.x == 0)
		{
			return 90;
		}
		else if(Pos.y == 0)
		{
			return 180;
		}
		else if(Pos.x == ofGetWidth())
		{
			return -90;
		}
		else if(Pos.y == ofGetHeight())
		{
			return 0;
		}
	}
}