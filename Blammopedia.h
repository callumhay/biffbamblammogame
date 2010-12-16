#ifndef __BLAMMOPEDIA_H__
#define __BLAMMOPEDIA_H__

#include "BlammoEngine/BasicIncludes.h"

#include "GameModel/GameItem.h"
#include "GameModel/LevelPiece.h"

#include <string>
#include <map>

class Texture2D;

class Blammopedia {

public:
	class Entry {
		friend class Blammopedia;
	public:
		Entry(const std::string& filename): filename(filename), isLocked(true) {}
		virtual ~Entry() {};

		void SetIsLocked(bool locked) { this->isLocked = locked; }
		bool GetIsLocked() const { return this->isLocked; }

		const std::string& GetName() const { return this->name; }
		const std::string& GetDescription() const { return this->description; }

	protected:
		// Item, block and status effect file keywords/syntax constants
		static const char* NAME_KEYWORD;
		static const char* ITEM_TEXTURE_KEYWORD;
		static const char* HUD_OUTLINE_TEXTURE_KEYWORD;
		static const char* HUD_FILL_TEXTURE_KEYWORD;
		static const char* DESCRIPTION_KEYWORD;

		std::string filename;

		bool isLocked;
		std::string name;
		std::string description;


		virtual bool PopulateFromFile() = 0;
		bool PopulateBaseValuesFromStream(std::istream& inStream);
	};

	class ItemEntry : public Entry {
		friend class Blammopedia;
	public:
		ItemEntry(const std::string& filename) : Entry(filename), 
			itemTexture(NULL), hudOutlineTexture(NULL), hudFillTexture(NULL) {};
		~ItemEntry();

		Texture2D* GetItemTexture() const { return this->itemTexture; }
		Texture2D* GetHUDOutlineTexture() const { return this->hudOutlineTexture; }
		Texture2D* GetHUDFillTexture() const { return this->hudFillTexture; }

	protected:
		bool PopulateFromFile();

	private:
		std::string itemTextureFilename;
		std::string hudOutlineTextureFilename;
		std::string hudFillTextureFilename;
		Texture2D* itemTexture;				// The item texture (texture that is shown when the item drops)
		Texture2D* hudOutlineTexture;
		Texture2D* hudFillTexture;
	};

	class BlockEntry : public Entry {
		friend class Blammopedia;
	public:
		BlockEntry(const std::string& filename) : Entry(filename) {}
		~BlockEntry() {};
	};
	class StatusEffectEntry : public Entry {
	public:
		StatusEffectEntry(const std::string& filename) : Entry(filename) {}
		~StatusEffectEntry() {};
	};

	~Blammopedia();
	static Blammopedia* BuildFromBlammopediaFile(const std::string &filepath);

	Blammopedia::ItemEntry*  GetItemEntry(const GameItem::ItemType& itemType) const;
	Blammopedia::BlockEntry* GetBlockEntry(const LevelPiece::LevelPieceType& blockType) const;
	Blammopedia::StatusEffectEntry* GetStatusEffectEntry(const LevelPiece::PieceStatus& statusType) const;

	bool WriteAsEntryStatusFile(const std::string &filepath) const;


private:
	// Blammopedia entry lock file keywords/syntax constants
	static const char* ITEM_ENTRIES;
	static const char* BLOCK_ENTRIES;
	static const char* STATUS_EFFECT_ENTRIES;

	std::map<GameItem::ItemType, Blammopedia::ItemEntry*> itemEntries;
	std::map<LevelPiece::LevelPieceType, Blammopedia::BlockEntry*> blockEntries;
	std::map<LevelPiece::PieceStatus, Blammopedia::StatusEffectEntry*> statusEffectEntries;

	Blammopedia();
	bool InitializeEntries();

	static bool ReadItemEntires(std::istream& inStream, std::map<GameItem::ItemType, bool>& itemStatusMap);
	static bool ReadBlockEntries(std::istream& inStream, std::map<LevelPiece::LevelPieceType, bool>& blockStatusMap);
	static bool ReadStatusEffectEntries(std::istream& inStream, std::map<LevelPiece::PieceStatus, bool>& statusEffectStatusMap);
		
	DISALLOW_COPY_AND_ASSIGN(Blammopedia);
};

inline Blammopedia::ItemEntry* Blammopedia::GetItemEntry(const GameItem::ItemType& itemType) const {
	std::map<GameItem::ItemType, Blammopedia::ItemEntry*>::const_iterator findIter = this->itemEntries.find(itemType);
	if (findIter == this->itemEntries.end()) {
		assert(false);
		return NULL;
	}
		
	return findIter->second;
}

inline Blammopedia::BlockEntry* Blammopedia::GetBlockEntry(const LevelPiece::LevelPieceType& blockType) const {
	std::map<LevelPiece::LevelPieceType, Blammopedia::BlockEntry*>::const_iterator findIter = this->blockEntries.find(blockType);
	if (findIter == this->blockEntries.end()) {
		assert(false);
		return NULL;
	}
		
	return findIter->second;
}

inline Blammopedia::StatusEffectEntry* Blammopedia::GetStatusEffectEntry(const LevelPiece::PieceStatus& statusType) const {
	std::map<LevelPiece::PieceStatus, Blammopedia::StatusEffectEntry*>::const_iterator findIter = this->statusEffectEntries.find(statusType);
	if (findIter == this->statusEffectEntries.end()) {
		assert(false);
		return NULL;
	}
		
	return findIter->second;
}

#endif // __BLAMMOPEDIAENTRY_H__