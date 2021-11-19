#pragma once
#include "GameObject.h"

class Model;
class Shader;

class Player : public GameObject, public enable_shared_from_this<Player>
{
private:
	Player();
	Player(const Player& Rhs);
public:
	~Player();

public:
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;

public:
	HRESULT Initialize(ID3D11Device* _Device);
public:
	static shared_ptr<Player> Create(ID3D11Device* _Device);

private:
	shared_ptr<Model> model;
	shared_ptr<Shader> shader = nullptr;

};

