/**
 * Blammopedia.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
		Entry(const std::string& filename): filename(filename), isLocked(true), hasBeenViewed(false) {}
		virtual ~Entry() {};

		void SetIsLocked(bool locked) { this->isLocked = locked; }
		bool GetIsLocked() const { return this->isLocked; }

        void SetHasBeenViewed(bool viewed) { this->hasBeenViewed = viewed; }
        bool GetHasBeenViewed() const { return this->hasBeenViewed; }

		const std::string& GetName() const { return this->name; }
		const std::string& GetDescription() const { return this->description; }
        const std::string& GetFinePrint() const { return this->finePrint; }

	protected:
		// Item, block and status effect file keywords/syntax constants
		static const char* NAME_KEYWORD;
		static const char* ITEM_TEXTURE_KEYWORD;
		static const char* HUD_OUTLINE_TEXTURE_KEYWORD;
		static const char* HUD_FILL_TEXTURE_KEYWORD;
		static const char* DESCRIPTION_KEYWORD;
        static const char* FINEPRINT_KEYWORD;
        static const char* DISPLAY_TEXTURE_KEYWORD;

		std::string filename;

		bool isLocked;
        bool hasBeenViewed;
		std::string name;
		std::string description;
        std::string finePrint;

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
		BlockEntry(const std::string& filename) : Entry(filename), blockTexture(NULL) {}
		~BlockEntry();

        Texture2D* GetBlockTexture() const { return this->blockTexture; }

	protected:
		bool PopulateFromFile();

    private:
        std::string blockTextureFilename;
        Texture2D* blockTexture;
        //Texture2D* altBlockTexture;

	};

	class MiscEntry : public Entry {
	public:
		MiscEntry(const std::string& filename) : Entry(filename) {}
		~MiscEntry() {};
	};

	typedef std::map<GameItem::ItemType, Blammopedia::ItemEntry*> ItemEntryMap;
	typedef std::map<LevelPiece::LevelPieceType, Blammopedia::BlockEntry*> BlockEntryMap;
	typedef std::map<LevelPiece::PieceStatus, Blammopedia::MiscEntry*> MiscEntryMap;

	typedef ItemEntryMap::iterator	ItemEntryMapIter;
	typedef BlockEntryMap::iterator	BlockEntryMapIter;
	typedef MiscEntryMap::iterator  MiscEntryMapIter;

	typedef ItemEntryMap::const_iterator   ItemEntryMapConstIter;
	typedef BlockEntryMap::const_iterator  BlockEntryMapConstIter;
	typedef MiscEntryMap::const_iterator   MiscEntryMapConstIter;

	~Blammopedia();
	
	Blammopedia::ItemEntry*  GetItemEntry(const GameItem::ItemType& itemType) const;
	Blammopedia::BlockEntry* GetBlockEntry(const LevelPiece::LevelPieceType& blockType) const;
	Blammopedia::MiscEntry*  GetMiscEntry(const LevelPiece::PieceStatus& statusType) const;

    void UnlockItem(const GameItem::ItemType& itemType);
    void UnlockBlock(const LevelPiece::LevelPieceType& blockType);

	const Blammopedia::ItemEntryMap& GetItemEntries() const;
	const Blammopedia::BlockEntryMap& GetBlockEntries() const;
	const Blammopedia::MiscEntryMap& GetMiscEntries() const;

    bool HasUnviewed() const;
    bool HasUnviewedBlocks() const;
    bool HasUnviewedItems() const;

    static Blammopedia* BuildFromBlammopediaFile(const std::string &filepath);
	bool WriteAsEntryStatusFile() const;

	const Texture2D* GetLockedItemTexture() const;

private:
    std::string blammopediaFile;

	ItemEntryMap itemEntries;
	BlockEntryMap blockEntries;
	MiscEntryMap miscEntries;

	Texture2D* lockedItemTexture;

	Blammopedia();
	bool InitializeEntries();

	static bool ReadItemEntires(std::istream& inStream, std::map<GameItem::ItemType, std::pair<bool, bool> >& itemStatusMap);
	static bool ReadBlockEntries(std::istream& inStream, std::map<LevelPiece::LevelPieceType, std::pair<bool, bool> >& blockStatusMap);
	//static bool ReadMiscEntries(std::istream& inStream, std::map<LevelPiece::PieceStatus, bool>& miscStatusMap);
		
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
    if (blockType == LevelPiece::Empty) {
        return NULL;
    }
	std::map<LevelPiece::LevelPieceType, Blammopedia::BlockEntry*>::const_iterator findIter = this->blockEntries.find(blockType);
	if (findIter == this->blockEntries.end()) {
		assert(false);
		return NULL;
	}
		
	return findIter->second;
}

inline Blammopedia::MiscEntry* Blammopedia::GetMiscEntry(const LevelPiece::PieceStatus& statusType) const {
	std::map<LevelPiece::PieceStatus, Blammopedia::MiscEntry*>::const_iterator findIter = this->miscEntries.find(statusType);
	if (findIter == this->miscEntries.end()) {
		assert(false);
		return NULL;
	}
		
	return findIter->second;
}

inline void Blammopedia::UnlockItem(const GameItem::ItemType& itemType) {
    Blammopedia::ItemEntry* itemEntry = this->GetItemEntry(itemType);
    assert(itemEntry != NULL);
    itemEntry->SetIsLocked(false);
}

inline void Blammopedia::UnlockBlock(const LevelPiece::LevelPieceType& blockType) {
    Blammopedia::BlockEntry* blockEntry = this->GetBlockEntry(blockType);
    if (blockEntry == NULL) {
        return;
    }
    blockEntry->SetIsLocked(false);
}

inline const Blammopedia::ItemEntryMap& Blammopedia::GetItemEntries() const {
	return this->itemEntries;
}

inline const Blammopedia::BlockEntryMap& Blammopedia::GetBlockEntries() const {
	return this->blockEntries;
}

inline const Blammopedia::MiscEntryMap& Blammopedia::GetMiscEntries() const {
	return this->miscEntries;
}

inline const Texture2D* Blammopedia::GetLockedItemTexture() const {
	return this->lockedItemTexture;
}

#endif // __BLAMMOPEDIAENTRY_H__