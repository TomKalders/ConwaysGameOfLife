#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <string>

class BaseEffect
{
public:
	BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~BaseEffect();

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;
	ID3DX11EffectMatrixVariable* GetWorldViewProjMatrix() const;
	ID3DX11EffectMatrixVariable* GetWorldMatrix() const;
	ID3DX11EffectMatrixVariable* GetViewInverseMatrix() const;
	ID3DX11EffectScalarVariable* GetFilterMethod() const;

protected:
	//Effects library variables
	///////////////////////////
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;

	//Projection Matrices
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectMatrixVariable* m_pMatWorldVariable;
	ID3DX11EffectMatrixVariable* m_pMatViewInverseVariable;

	//FilterMode
	ID3DX11EffectScalarVariable* m_pFilterMethod;
	
	void ShowEffectVarWarning(ID3DX11EffectVariable* var, const std::wstring& warning) const;
};

