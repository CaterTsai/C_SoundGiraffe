#ifndef OFX_GIRAFFE
#define OFX_GIRAFFE

#include "ofxAnimatableOfPoint.h"
#include "ofxAnimatableFloat.h"


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
		
		_LeftNeckVec.x = 0;
		_LeftNeckVec.y = _DrawStartPos.y + fLength - _DrawStartPos.y;
		_LeftNeckVec.normalize();

		_RightNeckVec.x = (_DrawStartPos.x + (0.3 * _fScale)) - (_DrawStartPos.x + (0.125 * _fScale));
		_RightNeckVec.y = (_DrawStartPos.y + _fNeckLength) - (_DrawStartPos.y - (0.05 * _fScale));
		_RightNeckVec.normalize();

		//Remove Spot
		bool bHaveRemove_ = false;
		auto Iter_ = _SpotsList.begin();
		while(Iter_ != _SpotsList.end())
		{
			if(Iter_->StartPos.y > fLength)
			{
				Iter_ = _SpotsList.erase(Iter_);
				bHaveRemove_ = true;
			}
			else
			{
				Iter_++;
			}
		}

		if(bHaveRemove_)
		{
			return;
		}

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

class GiraffeElement
{
public:
	void setup(ofVec2f StartPos, eANIM_MOVE_STATE eState, float fSize);
	void update(float fDelta);
	void draw();

private:
	//Giraffe
	DynamicGiraffe	_DyanmicGiraffe;

	//Animatable
	eANIM_MOVE_STATE		_eMoveState;
	ofxAnimatableFloat		_AnimRotate;
	ofxAnimatableOfPoint	_AnimMove;
};

#endif // !OFX_GIRAFFE
