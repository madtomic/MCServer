
// Prefab.h

/*
Declares the cPrefab class, representing a cPiece descendant for the cPieceGenerator that
uses a prefabricate in a cBlockArea for drawing itself.
The class can be constructed from data that is stored directly in the executable, in a sPrefabDef structure
declared in this file as well; the Gallery server exports areas in this format.
*/





#pragma once

#include "PieceGenerator.h"
#include "../BlockArea.h"





// fwd:
class cChunkDesc;





class cPrefab :
	public cPiece
{
public:
	struct sDef
	{
		int m_SizeX;
		int m_SizeY;
		int m_SizeZ;
		
		/** The mapping between characters in m_Image and the blocktype / blockmeta.
		Format: "Char: BlockType: BlockMeta \n Char: BlockType : BlockMeta \n ..." */
		const char * m_CharMap;
		
		/** The actual image to be used for the prefab. Organized YZX (Y changes the least often).
		Each character represents a single block, the type is mapped through m_CharMap. */
		const char * m_Image;
		
		/** List of connectors.
		Format: "Type: X, Y, Z : Direction \n Type : X, Y, Z : Direction \n ...".
		Type is an arbitrary number, Direction is the BlockFace constant value (0 .. 5). */
		const char * m_Connectors;
		
		/** Bitmask specifying the allowed rotations.
		N rotations CCW are allowed if bit N is set. */
		int m_AllowedRotations;
		
		/** The merge strategy to use while drawing the prefab. */
		cBlockArea::eMergeStrategy m_MergeStrategy;
		
		/** If set to true, the prefab will extend its lowermost blocks until a solid block is found,
		thus creating a foundation for the prefab. This is used for houses to be "on the ground", as well as
		nether fortresses not to float. */
		bool m_ShouldExtendFloor;

		/** Chance of this piece being used, if no other modifier is active. */
		int m_DefaultWeight;
		
		/** Chances of this piece being used, per depth of the generated piece tree.
		The starting piece has a depth of 0, the pieces connected to it are depth 1, etc.
		The specified depth stands for the depth of the new piece (not the existing already-placed piece),
		so valid depths start at 1.
		Format: "Depth : Weight | Depth : Weight | Depth : Weight ..."
		Depths that are not specified will use the m_DefaultWeight value. */
		const char * m_DepthWeight;
		
		/** The weight to add to this piece's base per-depth chance if the previous piece is the same.
		Can be positive or negative.
		This is used e. g. to make nether bridges prefer spanning multiple segments or to penalize turrets next to each other. */
		int m_AddWeightIfSame;
	};
	
	cPrefab(const sDef & a_Def);
	
	/** Draws the prefab into the specified chunk, according to the placement stored in the PlacedPiece. */
	void Draw(cChunkDesc & a_Dest, const cPlacedPiece * a_Placement) const;
	
	/** Returns true if the prefab has any connector of the specified type. */
	bool HasConnectorType(int a_ConnectorType) const;
	
	/** Returns the weight (chance) of this prefab generating as the next piece after the specified placed piece.
	PiecePool implementations can use this for their GetPieceWeight() implementations. */
	int GetPieceWeight(const cPlacedPiece & a_PlacedPiece, const cPiece::cConnector & a_ExistingConnector) const;

protected:
	/** Packs complete definition of a single block, for per-letter assignment. */
	struct sBlockTypeDef
	{
		BLOCKTYPE  m_BlockType;
		NIBBLETYPE m_BlockMeta;
	};
	
	/** Maps letters in the sDef::m_Image onto a sBlockTypeDef block type definition. */
	typedef sBlockTypeDef CharMap[256];
	
	/** Maps generator tree depth to weight. */
	typedef std::map<int, int> cDepthWeight;
	
	
	/** The cBlockArea that contains the block definitions for the prefab.
	The index identifies the number of CCW rotations applied (0 = no rotation, 1 = 1 CCW rotation, ...). */
	cBlockArea m_BlockArea[4];
	
	/** The size of the prefab */
	Vector3i m_Size;
	
	/** The hitbox of the prefab. In first version is the same as the m_BlockArea dimensions */
	cCuboid m_HitBox;
	
	/** The connectors through which the piece connects to other pieces */
	cConnectors m_Connectors;
	
	/** Bitmask, bit N set -> N rotations CCW supported */
	int m_AllowedRotations;
	
	/** The merge strategy to use when drawing the prefab into a block area */
	cBlockArea::eMergeStrategy m_MergeStrategy;

	/** If set to true, the prefab will extend its lowermost blocks until a solid block is found,
	thus creating a foundation for the prefab. This is used for houses to be "on the ground", as well as
	nether fortresses not to float. */
	bool m_ShouldExtendFloor;

	/** Chance of this piece being used, if no other modifier is active. */
	int m_DefaultWeight;
	
	/** Chances of this piece being used, per depth of the generated piece tree.
	The starting piece has a depth of 0, the pieces connected to it are depth 1, etc.
	The specified depth stands for the depth of the new piece (not the existing already-placed piece),
	so valid depths start at 1.
	Depths that are not specified will use the m_DefaultWeight value. */
	cDepthWeight m_DepthWeight;
	
	/** The weight to add to this piece's base per-depth chance if the previous piece is the same.
	Can be positive or negative.
	This is used e. g. to make nether bridges prefer spanning multiple segments or to penalize turrets next to each other. */
	int m_AddWeightIfSame;
	
	
	// cPiece overrides:	
	virtual cConnectors GetConnectors(void) const override;
	virtual Vector3i GetSize(void) const override;
	virtual cCuboid GetHitBox(void) const override;
	virtual bool CanRotateCCW(int a_NumRotations) const override;
	
	/** Parses the CharMap in the definition into a CharMap binary data used for translating the definition into BlockArea. */
	void ParseCharMap(CharMap & a_CharMapOut, const char * a_CharMapDef);
	
	/** Parses the Image in the definition into m_BlockArea[0]'s block types and metas, using the specified CharMap. */
	void ParseBlockImage(const CharMap & a_CharMap, const char * a_BlockImage);
	
	/** Parses the connectors definition text into m_Connectors member. */
	void ParseConnectors(const char * a_ConnectorsDef);
	
	/** Parses the per-depth weight into m_DepthWeight member. */
	void ParseDepthWeight(const char * a_DepthWeightDef);
};




