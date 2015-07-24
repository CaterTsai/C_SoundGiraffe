#pragma once

#include "ofMain.h"

#include "ofxAnimatableFloat.h"
#include "ofxAnimatableOfPoint.h"

static const int cNUM_OF_FFT_BANDS = 512;
static const int cNUM_OF_USED_BANDS = 32;
static const float cBASE_GIRAFFE_LENGTH = 150;

#pragma region Dynamic Giraffe
typedef struct _stSpotElement
{
	float	fStartLength_, fEndLength_;
	ofVec2f StartPos, EndPos;
	bool	bIsLeft;

	void update(ofVec2f StartP, ofVec2f Vec)
	{
		StartPos = StartP + Vec * fStartLength_;
		EndPos = StartP + Vec * fEndLength_;
	}

	void draw(float fScale)
	{
		int iLeft_ = (bIsLeft)?1:-1;

		ofBezier(	StartPos.x, StartPos.y,
					StartPos.x + iLeft_ * (0.1 * fScale), StartPos.y,
					EndPos.x + iLeft_ * (0.1 * fScale), EndPos.y,
					EndPos.x, EndPos.y
		);
	}

}stSpotElement;

class DynamicGiraffe
{
public:
	void setup(float fScale = 200)
	{
		_fScale = fScale;
		_fNeckLength = 0.0;
		_fSpotMaxSize = fScale * 0.5;

		_Outline.clear();
		_Outline.setFilled(false);
		_Outline.setStrokeWidth(3.0);
		_Outline.setStrokeColor(ofColor(255));
				
		_Party.set(255, 150, 3);
		_Party2.set(255);

		//_DrawStartPos.set(-0.15 * fScale, -(fScale * 0.95));
		_DrawStartPos.set(0, (0.375 * fScale));
		ofPoint StartPos_ = _DrawStartPos;
		
		_NeckLeftPos = _DrawStartPos;
		_NeckRightPos = ofVec2f(_DrawStartPos.x + (0.125 * _fScale), _DrawStartPos.y);

		_Outline.moveTo(StartPos_);
				
		//Mouse
		_Outline.bezierTo(	StartPos_.x - (0.4 * fScale), StartPos_.y, 
							StartPos_.x - (0.4 * fScale), StartPos_.y - (0.1 * fScale),
							StartPos_.x, StartPos_.y - (0.2 * fScale));

		//Tongue
		_TongueStart = ofBezierPoint(	StartPos_,
										ofPoint(StartPos_.x - (0.4 * fScale), StartPos_.y),
										ofPoint(StartPos_.x - (0.4 * fScale), StartPos_.y - (0.1 * fScale)),
										ofPoint(StartPos_.x, StartPos_.y - (0.2 * fScale)),
										0.2
										);

		_TongueEnd = ofBezierPoint(	StartPos_,
									ofPoint(StartPos_.x - (0.4 * fScale), StartPos_.y),
									ofPoint(StartPos_.x - (0.4 * fScale), StartPos_.y - (0.1 * fScale)),
									ofPoint(StartPos_.x, StartPos_.y - (0.2 * fScale)),
									0.3
									);
		

		//Horn
		//StartPos_.set(StartPos_.x, StartPos_.y - (0.2 * fScale));
		_Outline.bezierTo( StartPos_.x + (0.05 * fScale), StartPos_.y - (0.35 * fScale),
						StartPos_.x + (0.125 * fScale), StartPos_.y - (0.375 * fScale),
						StartPos_.x + (0.125 * fScale), StartPos_.y - (0.2 * fScale));

		//Ear
		//StartPos_.set(StartPos_.x + (0.125 * fScale), StartPos_.y);
		_Outline.bezierTo( StartPos_.x + (0.275 * fScale), StartPos_.y - (0.2 * fScale),
						StartPos_.x + (0.275 * fScale), StartPos_.y - (0.05 * fScale),
						StartPos_.x + (0.125 * fScale), StartPos_.y - (0.05 * fScale));
	}
	void update(float fLength)
	{
		_fNeckLength = fLength - _DrawStartPos.y;
		_fNeckLength = _fNeckLength < 0?0:_fNeckLength;

		_LeftNeckVec.x = 0;
		_LeftNeckVec.y = _DrawStartPos.y + fLength - _DrawStartPos.y;
		_LeftNeckVec.normalize();

		_RightNeckVec.x = (_DrawStartPos.x + (0.3 * _fScale)) - (_DrawStartPos.x + (0.125 * _fScale));
		_RightNeckVec.y = (_DrawStartPos.y + _fNeckLength) - (_DrawStartPos.y - (0.05 * _fScale));
		_RightNeckVec.normalize();

		//Remove Spot
		//bool bHaveRemove_ = false;
		//auto Iter_ = _SpotsList.begin();
		//while(Iter_ != _SpotsList.end())
		//{
		//	if(Iter_->StartPos.y > fLength)
		//	{
		//		Iter_ = _SpotsList.erase(Iter_);
		//		bHaveRemove_ = true;
		//	}
		//	else
		//	{
		//		Iter_++;
		//	}
		//}

		//if(bHaveRemove_)
		//{
		//	return;
		//}

		//update spot
		for(auto& Iter_ : _SpotsList)
		{
			if(Iter_.bIsLeft)
			{
				Iter_.update(_NeckLeftPos, _LeftNeckVec);
			}
			else
			{
				Iter_.update(_NeckRightPos, _RightNeckVec);
			}
			
		}

		//Add spot
		float fFreeLength_ = _fNeckLength;
		float fNowPosY_ = _DrawStartPos.y;
		if(_SpotsList.size() > 0)
		{
			fFreeLength_ -= (_SpotsList.back().EndPos.y - _DrawStartPos.y);
			fNowPosY_ = _SpotsList.back().EndPos.y;
		}

		if(fFreeLength_ < _fSpotMaxSize)
		{
			//space is not enough
			return;
		}

		stSpotElement NewSpot_;
		NewSpot_.bIsLeft = (ofRandom(-1, 1) > 0.0);		
		if(NewSpot_.bIsLeft)
		{
			float fR_ = (fNowPosY_ - _DrawStartPos.y)/_LeftNeckVec.y;
			ofVec2f StartPos_ = _NeckLeftPos + (_LeftNeckVec * fR_);
			ofVec2f EndPos_ = StartPos_ + _LeftNeckVec * _fSpotMaxSize;

			NewSpot_.StartPos = StartPos_.getInterpolated(EndPos_, ofRandom(0.1, 0.4));
			NewSpot_.EndPos = StartPos_.getInterpolated(EndPos_, ofRandom(0.6, 0.9));
			NewSpot_.fStartLength_ = _NeckLeftPos.distance(NewSpot_.StartPos);
			NewSpot_.fEndLength_ = _NeckLeftPos.distance(NewSpot_.EndPos);
		}
		else
		{
			float fR_ = (fNowPosY_ - _DrawStartPos.y)/_RightNeckVec.y;
			ofVec2f StartPos_ = _NeckRightPos + (_RightNeckVec * fR_);
			ofVec2f EndPos_ = StartPos_ + _RightNeckVec * _fSpotMaxSize;

			NewSpot_.StartPos = StartPos_.getInterpolated(EndPos_, ofRandom(0.1, 0.4));
			NewSpot_.EndPos = StartPos_.getInterpolated(EndPos_, ofRandom(0.6, 0.9));
			NewSpot_.fStartLength_ = _NeckRightPos.distance(NewSpot_.StartPos);
			NewSpot_.fEndLength_ = _NeckRightPos.distance(NewSpot_.EndPos);
		}
		
		_SpotsList.push_back(NewSpot_);
	}

	void draw()
	{
		ofPushStyle();
		ofSetColor(255);
		_Outline.draw();

		//Neck
		ofSetLineWidth(3);
		ofLine( _DrawStartPos.x + (0.125 * _fScale), _DrawStartPos.y - (0.05 * _fScale),
				_DrawStartPos.x + (0.3 * _fScale), _DrawStartPos.y + _fNeckLength);


		ofLine( _DrawStartPos.x, _DrawStartPos.y,
				_DrawStartPos.x, _DrawStartPos.y + _fNeckLength);

		//Tongue
		ofBezier(	_TongueStart.x, _TongueStart.y,
					_TongueStart.x - (0.075 * _fScale), _TongueStart.y + (0.05 * _fScale),
					_TongueEnd.x - (0.075 * _fScale), _TongueEnd.y + (0.05 * _fScale),
					_TongueEnd.x, _TongueEnd.y
					);


		if(_fNeckLength > ofGetHeight() * 0.2 && _fNeckLength < ofGetHeight() * 0.5)
		{
			ofSetColor(_Party);
		}
		else if(_fNeckLength >= ofGetHeight() * 0.5)
		{
			ofSetColor(_Party2);
		}
		else
		{
			ofSetColor(8, 143, 217);
		}
		//Eyes
		ofCircle(_DrawStartPos.x, _DrawStartPos.y - (0.1 * _fScale), (0.025 * _fScale));
		
		//Spots
		for(auto& Iter_ : _SpotsList)
		{
			Iter_.draw(_fScale);
		}

		ofPopStyle();
	}

	float getLength()
	{
		return _fNeckLength + _DrawStartPos.y;
	}
private:
	ofPath		_Outline;
	ofPoint		_TongueStart, _TongueEnd;
	float		_fScale;
	ofPoint		_DrawStartPos;

	float			_fNeckLength, _fSpotMaxSize;
	ofVec2f			_NeckLeftPos, _NeckRightPos;
	list<stSpotElement>	_SpotsList;
	ofVec2f			_LeftNeckVec, _RightNeckVec;
	
	ofColor		_Party, _Party2;
};
#pragma endregion

#pragma region Nyan Cat

typedef struct _stNyanCat
{
	void load()
	{
		ofDirectory	Dir_("cat/");
		Dir_.allowExt("png");
		int iNum_ = Dir_.listDir();
		_CatImg.resize(iNum_);
		for(int idx_ = 0; idx_ < iNum_; idx_++)
		{
			_CatImg[idx_].loadImage(Dir_.getPath(idx_));
		}
	}
	vector<ofImage>		_CatImg;

}stNyanCat;

class Rainbow
{
public:
	Rainbow()
	{
		_Color[0] = ofColor(255, 0, 0);
		_Color[1] = ofColor(255, 169, 0);
		_Color[2] = ofColor(255, 255, 0);
		_Color[3] = ofColor(65, 255, 0);
		_Color[4] = ofColor(0, 165, 255);
		_Color[5] = ofColor(120, 70, 255);
	}

	void draw(ofVec2f StartPos, ofVec2f EndPos, float fWidth, float fHeight)
	{
		float fTotalLength_ = EndPos.x - StartPos.x;
		float fRainbowHeight_ = fHeight * 0.6;

		int iNumRainbow_ = floor(fTotalLength_ / fWidth);
		

		ofVec2f Start_ = StartPos;
		for(int idx_ = 0; idx_ < iNumRainbow_; ++idx_)
		{
			float fAlpha_ = 255 * pow(0.8, iNumRainbow_ - idx_);
			if(idx_ % 2 == 0)
			{
				Start_.y += fRainbowHeight_ * 0.1;
			}
			else
			{
				Start_.y -= fRainbowHeight_ * 0.1;
			}

			ofPushMatrix();
			ofTranslate(Start_);
			{
				ofVec2f DrawPos_(-fWidth/2, -fRainbowHeight_/2);
				float fUnitHeight_ = fRainbowHeight_ / 6;
			
				
				for(int idx_ = 0; idx_ < 6; idx_++)
				{
					ofSetColor(_Color[idx_], fAlpha_);
				
					ofRect(DrawPos_, fWidth, fUnitHeight_);
					DrawPos_.y += fUnitHeight_;
				}
			}
			ofPopMatrix();

			Start_.x += fWidth;
		}

		//Short Rainbow
		if(iNumRainbow_ % 2 == 0)
		{
			Start_.y += fRainbowHeight_ * 0.1;
		}
		else
		{
			Start_.y -= fRainbowHeight_ * 0.1;
		}
		float fShortRainbowWidth_ = fTotalLength_ - iNumRainbow_ * fWidth;
		ofPushMatrix();
		ofTranslate(Start_);
		{
			ofVec2f DrawPos_(-fWidth/2, -fRainbowHeight_/2);
			float fUnitHeight_ = fRainbowHeight_ / 6;
							
			for(int idx_ = 0; idx_ < 6; idx_++)
			{
				ofSetColor(_Color[idx_]);
				
				ofRect(DrawPos_, fShortRainbowWidth_, fUnitHeight_);
				DrawPos_.y += fUnitHeight_;
			}
		}
		ofPopMatrix();
	}

private:

	ofColor	_Color[6];
};

class NyanCatMove
{
public:
	NyanCatMove(ofVec2f StartPos, ofVec2f EndPos, float fDuractions)
		:_iFrameMax(5)
		,_bAlive(true)
		,_iFrameIdx(0)
		,_fSPF(1.0/8.0)
		,_fScale(ofRandom(0.5, 1.0))
	{
		_AnimMove.setPosition(StartPos);
		_AnimMove.setCurve(AnimCurve::LINEAR);
		_AnimMove.setDuration(fDuractions);
		_AnimMove.animateTo(EndPos);
	}

	void update(float fDelta)
	{
		if(!_bAlive)
		{
			return;
		}

		_AnimMove.update(fDelta);
		_fTimer -= fDelta;
		if(_fTimer <= 0.0)
		{
			_fTimer = _fSPF;
			_iFrameIdx = (_iFrameIdx + 1) % _iFrameMax;
		}

		if(_AnimMove.getPercentDone() == 1.0 && _AnimMove.hasFinishedAnimating())
		{
			_bAlive = false;
		}

	}

public:
	inline ofVec2f getPos()
	{
		return _AnimMove.getCurrentPosition();
	}

	inline int getFrameIdx()
	{
		return _iFrameIdx;
	}

	inline float getScale()
	{
		return _fScale;
	}

	inline bool getAlive()
	{
		return _bAlive;
	}

private:
	bool					_bAlive;
	ofxAnimatableOfPoint	_AnimMove;
	int						_iFrameIdx;
	const int				_iFrameMax;
	float					_fSPF, _fTimer;

	float					_fScale;
	
};

class NyanCatMgr
{
public:
	NyanCatMgr()
		:_bAutoAdd(false)
	{}

	void setup()
	{
		_NyanCat.load();		
	}

	void update(float fDelta)
	{
		//Cat Move
		for(auto& Iter_ : _NyanCatMove)
		{	
			Iter_.update(fDelta);
		}

		//Romove CatMove
		auto Iter_  = _NyanCatMove.begin();
		while(Iter_ != _NyanCatMove.end())
		{
			if(Iter_->getAlive())
			{
				Iter_++;
			}
			else
			{
				Iter_ = _NyanCatMove.erase(Iter_);
			}
		}

		//Auto
		if(_bAutoAdd)
		{
			_fAutoTimer -= fDelta;
			if(_fAutoTimer < 0.0)
			{
				_fAutoTimer = ofRandom(2.0, 4.0);
				this->addCat();
			}
		}
	}

	void draw()
	{
		ofPushStyle();

		//DrawCat
		for(auto& Iter_ : _NyanCatMove)
		{
			int iFrameIdx_ = Iter_.getFrameIdx();
			float fWidth_ = _NyanCat._CatImg[iFrameIdx_].width * Iter_.getScale();
			float fHeight_ = _NyanCat._CatImg[iFrameIdx_].height * Iter_.getScale();
			
			//Draw Rainbow
			ofVec2f DrawPos_ = Iter_.getPos();
			ofVec2f StartPos_(0, DrawPos_.y);
			_Rainbow.draw(StartPos_, DrawPos_, fWidth_/4, fHeight_);

			//Draw Cat
			ofPushMatrix();
			ofTranslate(Iter_.getPos());
			{
				ofSetColor(255);
				_NyanCat._CatImg[iFrameIdx_].draw(-fWidth_/2, -fHeight_/2, fWidth_, fHeight_);
			}
			ofPopMatrix();
		}

		ofPopStyle();
	}

public:
	inline void start()
	{
		_bAutoAdd = true;
	}

	inline void stop()
	{
		_bAutoAdd = false;
	}

private:
	void addCat()
	{
		float fHeight_ = ofRandomHeight();
		NyanCatMove NewCat_(ofVec2f(0, fHeight_), ofVec2f(ofGetWidth(), fHeight_), ofRandom(3, 8));
		_NyanCatMove.push_back(NewCat_);
	}


private:
	bool				_bAutoAdd;
	float				_fAutoTimer;

	stNyanCat			_NyanCat;
	Rainbow				_Rainbow;
	list<NyanCatMove>	_NyanCatMove;

};

#pragma endregion

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	
	ofVec2f getPos(float fStartLength, float fPerc);
	float getRotate(ofVec2f Pos);
private:
	float			_fTimer;

	ofSoundPlayer	_SoundPlayer;
	float			_FFTBands[cNUM_OF_FFT_BANDS];
	float			_fTotalVol;

	float			_StartPos[cNUM_OF_USED_BANDS];
	float			_TotalLength, _fBacketWidth, _fGiraffeWidth;
	ofVec2f			_ArcVec1, _ArcVec2, _ArcVec3, _ArcVec4;
	ofxAnimatableFloat		_AnimPos;

	DynamicGiraffe	_DyanmicGiraffe[cNUM_OF_USED_BANDS];
	bool			_Direction[cNUM_OF_USED_BANDS];


	NyanCatMgr		_JazzCat;
	
};
