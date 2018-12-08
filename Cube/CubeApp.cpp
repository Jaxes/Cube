#include "CubeApp.h"

CubeApp::CubeApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mShapesVB(0), mShapesIB(0), Key(NONE), PressKey(false), wasPress(false), isDone(true), isMess(false), PressSpace(false),MessStep(50),t(0)//初始化
{
	mMainWndCaption = L"三阶魔方";
	
	m_target = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
	XMFLOAT3 _target(0.0f, 0.0f, 0.0f);
	XMFLOAT3 _up(0.0f, 1.0f, 0.0f);
	mCam.SetPosition(-35 / 2.0f, 35 / 2.0f, -35 / 2.0f);
	mCam.LookAt(mCam.GetPosition(), _target, _up);
	//设置初始的 世界坐标
	UINT WorldIndex[3][3][3] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26 };
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				XMMATRIX boxScale = XMMatrixScaling(3.0f, 3.0f, 3.0f);
				XMMATRIX boxOffset = XMMatrixTranslation(3.0f-i*3.0f, -3.0f+j*3.0f, -3.0f+k*3.0f);
				XMStoreFloat4x4(&mBoxWorld[WorldIndex[i][j][k]], XMMatrixMultiply(boxScale, boxOffset));
			}
		}
	}

	//灯光属性
	int k = 0;
	for (int i = 0; i < 2; i++, k++)
	{
		mDirLights[k].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		mDirLights[k].Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		mDirLights[k].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		mDirLights[k].Direction = XMFLOAT3(2.0f - 4.0f*i, 0.0f, 0.0f);
	}
	for (int i = 0; i < 2; i++, k++)
	{
		mDirLights[k].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		mDirLights[k].Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		mDirLights[k].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		mDirLights[k].Direction = XMFLOAT3(0.0f, 2.0f - 4.0f*i, 0.0f);
	}
	for (int i = 0; i < 2; i++, k++)
	{
		mDirLights[k].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		mDirLights[k].Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		mDirLights[k].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		mDirLights[k].Direction = XMFLOAT3(0.0f, 0.0f, 2.0f - 4.0f*i);
	}

	//材料属性
	mBoxMat.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	mTexSRV = 0;
}

CubeApp::~CubeApp()
{
	ReleaseCOM(mShapesVB);
	ReleaseCOM(mShapesIB);
	ReleaseCOM(mTexSRV);

	Effects::DestroyAll();
	InputLayouts::DestroyAll(); 
}

bool CubeApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	//载入纹理资源
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textrues/Textrues.png", 0, 0, &mTexSRV, 0));

	BuildShapeGeometryBuffers();

	return true;
}

void CubeApp::OnResize()
{
	D3DApp::OnResize();

	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void CubeApp::UpdateScene(float dt)
{
	if (!PressSpace&&isMess&&isDone)
	{
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = UP_KEY;
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = DOWN_KEY;
		}

		if (GetAsyncKeyState(VK_PRIOR) & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = PUP_KEY;
		}

		if (GetAsyncKeyState(VK_NEXT) & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = PDOWN_KEY;
		}

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = LEFT_KEY;
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = RIGHT_KEY;
		}

		if (GetAsyncKeyState('Q') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = Q_KEY;
		}

		if (GetAsyncKeyState('W') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = W_KEY;
		}

		if (GetAsyncKeyState('E') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = E_KEY;
		}

		if (GetAsyncKeyState('A') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = A_KEY;
		}

		if (GetAsyncKeyState('S') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = S_KEY;
		}

		if (GetAsyncKeyState('D') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = D_KEY;
		}

		if (GetAsyncKeyState('R') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = R_KEY;
		}

		if (GetAsyncKeyState('T') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = T_KEY;
		}

		if (GetAsyncKeyState('Y') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = Y_KEY;
		}

		if (GetAsyncKeyState('F') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = F_KEY;
		}

		if (GetAsyncKeyState('G') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = G_KEY;
		}

		if (GetAsyncKeyState('H') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = H_KEY;
		}

		if (GetAsyncKeyState('U') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = U_KEY;
		}

		if (GetAsyncKeyState('I') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = I_KEY;
		}

		if (GetAsyncKeyState('O') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = O_KEY;
		}

		if (GetAsyncKeyState('J') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = J_KEY;
		}

		if (GetAsyncKeyState('K') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = K_KEY;
		}

		if (GetAsyncKeyState('L') & 0x8000)
		{
			isDone = false;
			PressKey = true;
			Key = L_KEY;
		}
	}

	if (isMess&&!s.empty()&&isDone && (GetAsyncKeyState(VK_SPACE) & 0x8000))
	{
		PressSpace = true;
	}

	if (isMess&&s.empty()&&isDone && (GetAsyncKeyState(VK_F1) & 0x8000))
	{
		isMess = false;
		MessStep = 50;
	}

	//为了防止未转动pi/2时再次触发其他旋转
	if (!PressKey&&Key != NONE)
	{
		wasPress = true;
	}
}

void CubeApp::DrawScene()
{
	//用于正方体旋转的
	XMMATRIX Rotation;
	XMFLOAT4X4 FinalPos;

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 

	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	mCam.UpdateViewMatrix();
 
	XMMATRIX view     = mCam.View();
	XMMATRIX proj     = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();

	
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mCam.GetPosition());
 
	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light3TexTech;
	
	//随机打乱魔方
	if (!isMess)
	{
		if (!wasPress&&isDone)
		{
			switch (rand() % 24)
			{
			case 0:Key = Q_KEY; break;
			case 1:Key = W_KEY; break;
			case 2:Key = E_KEY; break;
			case 3:Key = R_KEY; break;
			case 4:Key = T_KEY; break;
			case 5:Key = Y_KEY; break;
			case 6:Key = U_KEY; break;
			case 7:Key = I_KEY; break;
			case 8:Key = O_KEY; break;
			case 9:Key = A_KEY; break;
			case 10:Key = S_KEY; break;
			case 11:Key = D_KEY; break;
			case 12:Key = F_KEY; break;
			case 13:Key = G_KEY; break;
			case 14:Key = H_KEY; break;
			case 15:Key = J_KEY; break;
			case 16:Key = K_KEY; break;
			case 17:Key = L_KEY; break;
			case 18:Key = PUP_KEY; break;
			case 19:Key = PDOWN_KEY; break;
			case 20:Key = UP_KEY; break;
			case 21:Key = DOWN_KEY; break;
			case 22:Key = LEFT_KEY; break;
			case 23:Key = RIGHT_KEY; break;
			}
			wasPress = true;
		}
	}

	//逆序法还原魔方
	if (PressSpace&&isDone)
	{
		isDone = false;
		wasPress = true;
		Key = s.top();
		s.pop();
		switch (Key)
		{
		case CubeApp::Q_KEY:
			Key = E_KEY;
			break;
		case CubeApp::W_KEY:
			Key = S_KEY;
			break;
		case CubeApp::E_KEY:
			Key = Q_KEY;
			break;
		case CubeApp::R_KEY:
			Key = Y_KEY;
			break;
		case CubeApp::T_KEY:
			Key = G_KEY;
			break;
		case CubeApp::Y_KEY:
			Key = R_KEY;
			break;
		case CubeApp::U_KEY:
			Key = O_KEY;
			break;
		case CubeApp::I_KEY:
			Key = K_KEY;
			break;
		case CubeApp::O_KEY:
			Key = U_KEY;
			break;
		case CubeApp::A_KEY:
			Key = D_KEY;
			break;
		case CubeApp::S_KEY:
			Key = W_KEY;
			break;
		case CubeApp::D_KEY:
			Key = A_KEY;
			break;
		case CubeApp::F_KEY:
			Key = H_KEY;
			break;
		case CubeApp::G_KEY:
			Key = T_KEY;
			break;
		case CubeApp::H_KEY:
			Key = F_KEY;
			break;
		case CubeApp::J_KEY:
			Key = L_KEY;
			break;
		case CubeApp::K_KEY:
			Key = I_KEY;
			break;
		case CubeApp::L_KEY:
			Key = J_KEY;
			break;
		case CubeApp::PUP_KEY:
			Key = PDOWN_KEY;
			break;
		case CubeApp::PDOWN_KEY:
			Key = PUP_KEY;
			break;
		case CubeApp::UP_KEY:
			Key = DOWN_KEY;
			break;
		case CubeApp::DOWN_KEY:
			Key = UP_KEY;
			break;
		case CubeApp::LEFT_KEY:
			Key = RIGHT_KEY;
			break;
		case CubeApp::RIGHT_KEY:
			Key = LEFT_KEY;
			break;
		}
	}
	//魔方旋转（为了实现动画效果，每次调用DrawScence函数只转动pi/400）
	if (wasPress)
	{
		switch (Key)
		{
		case Q_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._41) - 3.0f) < 0.1f&&mBoxWorld[i]._41 > 0.0f)
				{
					Rotation = XMMatrixRotationX(MathHelper::Pi / 400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case W_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._43) - 3.0f) < 0.1f&&mBoxWorld[i]._43 > 0.0f)
				{
					Rotation = XMMatrixRotationZ(MathHelper::Pi / -400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case E_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._41) - 3.0f) < 0.1f&&mBoxWorld[i]._41 > 0.0f)
				{
					Rotation = XMMatrixRotationX(MathHelper::Pi / -400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case R_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._41) - 0.0f) < 0.1f)
				{
					Rotation = XMMatrixRotationX(MathHelper::Pi / 400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case T_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._43) - 0.0f) < 0.1f)
				{
					Rotation = XMMatrixRotationZ(MathHelper::Pi / -400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case Y_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._41) - 0.0f) < 0.1f)
				{
					Rotation = XMMatrixRotationX(MathHelper::Pi / -400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case U_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._41) - 3.0f) < 0.1f&&mBoxWorld[i]._41 < 0.0f)
				{
					Rotation = XMMatrixRotationX(MathHelper::Pi / 400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case I_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._43) - 3.0f) < 0.1f&&mBoxWorld[i]._43 < 0.0f)
				{
					Rotation = XMMatrixRotationZ(MathHelper::Pi / -400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case O_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._41) - 3.0f) < 0.1f&&mBoxWorld[i]._41 < 0.0f)
				{
					Rotation = XMMatrixRotationX(MathHelper::Pi / -400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case A_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._42) - 3.0f) < 0.1f&&mBoxWorld[i]._42 < 0.0f)
				{
					Rotation = XMMatrixRotationY(MathHelper::Pi / 400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case S_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._43) - 3.0f) < 0.1f&&mBoxWorld[i]._43 > 0.0f)
				{
					Rotation = XMMatrixRotationZ(MathHelper::Pi / 400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case D_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._42) - 3.0f) < 0.1f&&mBoxWorld[i]._42 < 0.0f)
				{
					Rotation = XMMatrixRotationY(MathHelper::Pi / -400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case F_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._42) - 0.0f) < 0.1f)
				{
					Rotation = XMMatrixRotationY(MathHelper::Pi / 400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case G_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._43) - 0.0f) < 0.1f)
				{
					Rotation = XMMatrixRotationZ(MathHelper::Pi / 400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case H_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._42) - 0.0f) < 0.1f)
				{
					Rotation = XMMatrixRotationY(MathHelper::Pi / -400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case J_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._42) - 3.0f) < 0.1f&&mBoxWorld[i]._42 > 0.0f)
				{
					Rotation = XMMatrixRotationY(MathHelper::Pi / 400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case K_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._43) - 3.0f) < 0.1f&&mBoxWorld[i]._43 < 0.0f)
				{
					Rotation = XMMatrixRotationZ(MathHelper::Pi / 400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case L_KEY:
			for (int i = 0; i < 27; i++)
			{
				if (fabs(fabs(mBoxWorld[i]._42) - 3.0f) < 0.1f&&mBoxWorld[i]._42 > 0.0f)
				{
					Rotation = XMMatrixRotationY(MathHelper::Pi / -400.0f);
					FinalPos = mBoxWorld[i];
					XMMATRIX M = XMLoadFloat4x4(&FinalPos);
					M *= Rotation;
					XMStoreFloat4x4(&FinalPos, M);
					mBoxWorld[i] = FinalPos;
				}
			}
			t++;
			break;
		case UP_KEY:
			for (int i = 0; i < 27; i++)
			{
				Rotation = XMMatrixRotationX(MathHelper::Pi / 400.0f);
				FinalPos = mBoxWorld[i];
				XMMATRIX M = XMLoadFloat4x4(&FinalPos);
				M *= Rotation;
				XMStoreFloat4x4(&FinalPos, M);
				mBoxWorld[i] = FinalPos;
			}
			t++;
			break;
		case DOWN_KEY:
			for (int i = 0; i < 27; i++)
			{
				Rotation = XMMatrixRotationX(MathHelper::Pi / -400.0f);
				FinalPos = mBoxWorld[i];
				XMMATRIX M = XMLoadFloat4x4(&FinalPos);
				M *= Rotation;
				XMStoreFloat4x4(&FinalPos, M);
				mBoxWorld[i] = FinalPos;
			}
			t++;
			break;
		case LEFT_KEY:
			for (int i = 0; i < 27; i++)
			{
				Rotation = XMMatrixRotationY(MathHelper::Pi / -400.0f);
				FinalPos = mBoxWorld[i];
				XMMATRIX M = XMLoadFloat4x4(&FinalPos);
				M *= Rotation;
				XMStoreFloat4x4(&FinalPos, M);
				mBoxWorld[i] = FinalPos;
			}
			t++;
			break;
		case RIGHT_KEY:
			for (int i = 0; i < 27; i++)
			{
				Rotation = XMMatrixRotationY(MathHelper::Pi / 400.0f);
				FinalPos = mBoxWorld[i];
				XMMATRIX M = XMLoadFloat4x4(&FinalPos);
				M *= Rotation;
				XMStoreFloat4x4(&FinalPos, M);
				mBoxWorld[i] = FinalPos;
			}
			t++;
			break;
		case PUP_KEY:
			for (int i = 0; i < 27; i++)
			{
				Rotation = XMMatrixRotationZ(MathHelper::Pi / -400.0f);
				FinalPos = mBoxWorld[i];
				XMMATRIX M = XMLoadFloat4x4(&FinalPos);
				M *= Rotation;
				XMStoreFloat4x4(&FinalPos, M);
				mBoxWorld[i] = FinalPos;
			}
			t++;
			break;
		case PDOWN_KEY:
			for (int i = 0; i < 27; i++)
			{
				Rotation = XMMatrixRotationZ(MathHelper::Pi / 400.0f);
				FinalPos = mBoxWorld[i];
				XMMATRIX M = XMLoadFloat4x4(&FinalPos);
				M *= Rotation;
				XMStoreFloat4x4(&FinalPos, M);
				mBoxWorld[i] = FinalPos;
			}
			t++;
			break;
		case NONE:
			break;
		}
	}
	//转动200次时刚好总的转动角度为pi/2
	if (t > 200)
	{
		for (int i = 0; i < 27; i++)
		{
			CheckError(mBoxWorld[i]);
		}
		isDone = true;
		t = 0;
		if (!PressSpace&&(!s.empty()||s.empty()&&Key != UP_KEY && Key != DOWN_KEY && Key != LEFT_KEY && Key != RIGHT_KEY && Key != PUP_KEY && Key != PDOWN_KEY))
		{
			s.push(Key);
		}
		Key = NONE;
		wasPress = false;
		if (MessStep > 0)
		{
			MessStep--;
		}
		if (s.empty())
		{
			PressSpace = false;
		}
	}
	if (MessStep == 0)isMess = true;
	PressKey = false;
	

    D3DX11_TECHNIQUE_DESC techDesc;
    activeTexTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		//传顶点索引缓冲
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mShapesVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mShapesIB, DXGI_FORMAT_R32_UINT, 0);

		//根据正方体中心的世界坐标渲染正方体
		for (int i = 0; i < 27; i++)
		{
			XMMATRIX world = XMLoadFloat4x4(&mBoxWorld[i]);
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world * view * proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(mBoxMat);
			Effects::BasicFX->SetDiffuseMap(mTexSRV);
		
			activeTexTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
		}
    }
	//交换链
	HR(mSwapChain->Present(0, 0));
}

void CubeApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void CubeApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CubeApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}


void CubeApp::BuildShapeGeometryBuffers()
{
	GeometryGenerator::MeshData box;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

	//缓存连接顶点缓冲区中每个对象的顶点偏移量。
	mBoxVertexOffset      = 0;

	//缓存每个对象的索引计数。
	mBoxIndexCount      = box.Indices.size();

	//缓存连接索引缓冲区中每个对象的起始索引。
	mBoxIndexOffset      = 0;
	
	UINT totalVertexCount = box.Vertices.size();

	UINT totalIndexCount = mBoxIndexCount;

	//提取我们感兴趣的顶点元素并将所有网格的顶点打包到一个顶点缓冲区中。

	std::vector<Vertex::Basic32> vertices(totalVertexCount);
	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mShapesVB));

	//将所有网格的索引打包到一个索引缓冲区中。

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mShapesIB));
}

void CubeApp::CheckError(XMFLOAT4X4 & m)
{
	if (fabs(m._11) < 0.1f)m._11 = 0.0f;
	if (fabs(m._12) < 0.1f)m._12 = 0.0f;
	if (fabs(m._13) < 0.1f)m._13 = 0.0f;
	if (fabs(m._14) < 0.1f)m._14 = 0.0f;
	if (fabs(m._21) < 0.1f)m._21 = 0.0f;
	if (fabs(m._22) < 0.1f)m._22 = 0.0f;
	if (fabs(m._23) < 0.1f)m._23 = 0.0f;
	if (fabs(m._24) < 0.1f)m._24 = 0.0f;
	if (fabs(m._31) < 0.1f)m._31 = 0.0f;
	if (fabs(m._32) < 0.1f)m._32 = 0.0f;
	if (fabs(m._33) < 0.1f)m._33 = 0.0f;
	if (fabs(m._34) < 0.1f)m._34 = 0.0f;
	if (fabs(m._41) < 0.1f)m._41 = 0.0f;
	if (fabs(m._42) < 0.1f)m._42 = 0.0f;
	if (fabs(m._43) < 0.1f)m._43= 0.0f;
	if (fabs(m._44) < 0.1f)m._44 = 0.0f;

	if (fabs(fabs(m._11) - 3.0f) < 0.1f)m._11 = m._11 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._12) - 3.0f) < 0.1f)m._12 = m._12 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._13) - 3.0f) < 0.1f)m._13 = m._13 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._14) - 3.0f) < 0.1f)m._14 = m._14 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._21) - 3.0f) < 0.1f)m._21 = m._21 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._22) - 3.0f) < 0.1f)m._22 = m._22 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._23) - 3.0f) < 0.1f)m._23 = m._23 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._24) - 3.0f) < 0.1f)m._24 = m._24 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._31) - 3.0f) < 0.1f)m._31 = m._31 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._32) - 3.0f) < 0.1f)m._32 = m._32 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._33) - 3.0f) < 0.1f)m._33 = m._33 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._34) - 3.0f) < 0.1f)m._34 = m._34 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._41) - 3.0f) < 0.1f)m._41 = m._41 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._42) - 3.0f) < 0.1f)m._42 = m._42 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._43) - 3.0f) < 0.1f)m._43 = m._43 < 0 ? -3.0f : 3.0f;
	if (fabs(fabs(m._44) - 3.0f) < 0.1f)m._44 = m._44 < 0 ? -3.0f : 3.0f;
}
