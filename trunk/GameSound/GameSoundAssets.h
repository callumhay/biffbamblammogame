#ifndef _GAMESOUNDASSETS_H_
#define _GAMESOUNDASSETS_H_


class GameSoundAssets {

public:
	GameSoundAssets();
	~GameSoundAssets();

	void Tick(double dT);

	void LoadWorldSounds(int worldStyle);

private:
	// Global sound effect variables
	// ...

	// World sound effect variables
	// ...

	// Currently active event sounds
	// ...

	// Currently active mask sounds
	// ...

	void LoadGlobalSounds();

};
#endif