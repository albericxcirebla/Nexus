/*******************************************************************************************
 
			(c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2017] ++
			
			(c) Copyright Nexus Developers 2014 - 2017
			
			http://www.opensource.org/licenses/mit-license.php
  
*******************************************************************************************/

#ifndef NEXUS_CORE_BLOCK_H
#define NEXUS_CORE_BLOCK_H

#include <vector>

namespace Core
{
	class CBlockIndex; 
	
	/** Nodes collect new transactions into a block, hash them into a hash tree,
	 * and scan through nonce values to make the block's hash satisfy proof-of-work
	 * requirements.  When they solve the proof-of-work, they broadcast the block
	 * to everyone and the block is added to the block chain.  The first transaction
	 * in the block is a special one that creates a new coin owned by the creator
	 * of the block.
	 *
	 * Blocks are appended to blk0001.dat files on disk.  Their location on disk
	 * is indexed by CBlockIndex objects in memory.
	 */
	class CBlock
	{
	public:
		
		/* Core Block Header. */
		unsigned int nVersion;
		uint1024 hashPrevBlock;
		uint512 hashMerkleRoot;
		unsigned int nChannel;
		unsigned int nHeight;
		unsigned int nBits;
		uint64 nNonce;
		
		
		/* The Block Time set at Block Signature. */
		unsigned int nTime;

		
		/* Nexus: block signature */
		std::vector<unsigned char> vchBlockSig;
		
		
		/* Transactions for Current Block. */
		std::vector<CTransaction> vtx;

		
		/* Memory Only Data. */
		mutable std::vector<uint512> vMerkleTree;
		uint512 hashPrevChecksum;
		
		
		IMPLEMENT_SERIALIZE
		(
			READWRITE(this->nVersion);
			nVersion = this->nVersion;
			READWRITE(hashPrevBlock);
			READWRITE(hashMerkleRoot);
			READWRITE(nChannel);
			READWRITE(nHeight);
			READWRITE(nBits);
			READWRITE(nNonce);
			READWRITE(nTime);

			// ConnectBlock depends on vtx following header to generate CDiskTxPos
			if (!(nType & (SER_GETHASH|SER_BLOCKHEADERONLY)))
			{
				READWRITE(vtx);
				READWRITE(vchBlockSig);
			}
			else if (fRead)
			{
				const_cast<CBlock*>(this)->vtx.clear();
				const_cast<CBlock*>(this)->vchBlockSig.clear();
			}
		)

		CBlock();
		CBlock(unsigned int nVersionIn, uint1024 hashPrevBlockIn, unsigned int nChannelIn, unsigned int nHeightIn);


		/* Set block to a NULL state. */
		void SetNull()
		
		
		/* Set the Channel for block. */
		void SetChannel(unsigned int nNewChannel)

		
		/* Get the Channel block is produced from. */
		int GetChannel() const


		/* Check the NULL state of the block. */
		bool IsNull() const
		
		
		/* Get the prime number of the block. */
		CBigNum GetPrime() const

		
		/* Generate a Hash For the Block from the Header. */
		uint1024 GetHash() const
		
		
		/* Generate the Signature Hash Required After Block completes Proof of Work / Stake. */
		uint1024 SignatureHash() const;
		
		
		/* Return the Block's current UNIX Timestamp. */
		int64 GetBlockTime() const;
		
		
		/* Update the nTime of the current block. */
		void UpdateTime();
		
		
		/* Check flags for nPoS block. */
		bool IsProofOfStake() const;
		
		
		/* Check flags for PoW block. */
		bool IsProofOfWork() const;

		
		/* Generate the Merkle Tree from uint512 hashes. */
		uint512 BuildMerkleTree() const;

		
		/* Check that the Merkle branch matches hash tree. */
		static uint512 CheckMerkleBranch(uint512 hash, const std::vector<uint512>& vMerkleBranch, int nIndex);

	
		/* Write Block to Disk File. */
		bool WriteToDisk(unsigned int& nFileRet, unsigned int& nBlockPosRet);

		
		/* Read Block from Disk File. */
		bool ReadFromDisk(unsigned int nFile, unsigned int nBlockPos, bool fReadTransactions=true);
		
		
		/* Read Block from Disk File by Index Object. */
		bool ReadFromDisk(const CBlockIndex* pindex, bool fReadTransactions=true);


		/* Dump the Block data to Console / Debug.log. */
		void print() const;

		
		/* Disconnect all associated inputs from a block. */
		bool DisconnectBlock(LLD::CIndexDB& indexdb, CBlockIndex* pindex);
		
		
		/* Connect all associated inputs from a block. */
		bool ConnectBlock(LLD::CIndexDB& indexdb, CBlockIndex* pindex);
		
		
		/* Set the block as the best chain. */
		bool SetBestChain(LLD::CIndexDB& indexdb, CBlockIndex* pindexNew);
		
		
		/* Write block into Block Index Database. */
		bool AddToBlockIndex(unsigned int nFile, unsigned int nBlockPos);
		
		
		/* Check the validity of the block without connection to Blockchain. */
		bool CheckBlock() const;
		
		
		/* Verify the Proof of Work satisfies network requirements. */
		bool VerifyWork() const;
		
		
		/* Verify the Proof of Stake satisfies network requirements. */
		bool VerifyStake() const;
		
		
		/* Accept a block into the block chain. */
		bool AcceptBlock();
		
		
		/* Determine the Stake Weight of Given Block. */
		bool StakeWeight();
		
		
		/* Sign the block with the key that found the block. */
		bool SignBlock(const Wallet::CKeyStore& keystore);
		
		
		/* Check that the block signature is a valid signature. */
		bool CheckBlockSignature() const;
		
	};



	/** The block chain is a tree shaped structure starting with the
	 * genesis block at the root, with each block potentially having multiple
	 * candidates to be the next block.  pprev and pnext link a path through the
	 * main/longest chain.  A blockindex may have multiple pprev pointing back
	 * to it, but pnext will only point forward to the longest branch, or will
	 * be null if the block is not part of the longest chain.
	 */
	class CBlockIndex
	{
	public:
		const uint1024* phashBlock;
		CBlockIndex* pprev;
		CBlockIndex* pnext;
		unsigned int nFile;
		unsigned int nBlockPos;
		
		uint64 nChainTrust; // Nexus: trust score of block chain
		int64  nMint;
		int64  nMoneySupply;
		int64  nChannelHeight;
		int64  nReleasedReserve[4];
		int64  nCoinbaseRewards[3];
		
				
		/* Used to store the pending Checkpoint in Blockchain. 
			This is also another proof that this block is descendant 
			of most recent Pending Checkpoint. This helps Nexus
			deal with reorganizations of a Pending Checkpoint **/
		std::pair<unsigned int, uint1024> PendingCheckpoint;
		

		unsigned int nFlags;  // Nexus: block index flags
		uint64 nStakeModifier;

		// block header
		unsigned int nVersion;
		uint512 hashMerkleRoot;
		unsigned int nChannel;
		unsigned int nHeight;
		unsigned int nBits;
		uint64 nNonce;

		unsigned int nTime;

		CBlockIndex();
		CBlockIndex(unsigned int nFileIn, unsigned int nBlockPosIn, CBlock& block);

		CBlock GetBlockHeader() const;
		int GetChannel() const;
		uint1024 GetBlockHash() const;
		int64 GetBlockTime() const;
		uint64 GetBlockTrust() const;
		bool IsInMainChain() const;
		bool CheckIndex() const;
		bool EraseBlockFromDisk();
		bool IsProofOfWork() const;
		bool IsProofOfStake() const;
		std::string ToString() const;
		void print() const;
	};
	
	
	/** Used to marshal pointers into hashes for db storage. */
	class CDiskBlockIndex : public CBlockIndex
	{
	public:
		uint1024 hashPrev;
		uint1024 hashNext;

		IMPLEMENT_SERIALIZE
		(
			if (!(nType & SER_GETHASH))
				READWRITE(nVersion);

			if (!(nType & SER_LLD)) 
			{
				READWRITE(hashNext);
				READWRITE(nFile);
				READWRITE(nBlockPos);
				READWRITE(nMint);
				READWRITE(nMoneySupply);
				READWRITE(nFlags);
				READWRITE(nStakeModifier);
			}
			else
			{
				READWRITE(hashNext);
				READWRITE(nFile);
				READWRITE(nBlockPos);
				READWRITE(nMint);
				READWRITE(nMoneySupply);
				READWRITE(nChannelHeight);
				READWRITE(nChainTrust);
				
				READWRITE(nCoinbaseRewards[0]);
				READWRITE(nCoinbaseRewards[1]);
				READWRITE(nCoinbaseRewards[2]);
				READWRITE(nReleasedReserve[0]);
				READWRITE(nReleasedReserve[1]);
				READWRITE(nReleasedReserve[2]);

			}

			READWRITE(this->nVersion);
			READWRITE(hashPrev);
			READWRITE(hashMerkleRoot);
			READWRITE(nChannel);
			READWRITE(nHeight);
			READWRITE(nBits);
			READWRITE(nNonce);
			READWRITE(nTime);
			
		)
		
		
		/* Basic Disk Index Constructor. */
		CDiskBlockIndex()

		
		/* Disk Index Construct from Block Index. */
		explicit CDiskBlockIndex(CBlockIndex* pindex) : CBlockIndex(*pindex)


		/* Get the Block Hash. */
		uint1024 GetBlockHash() const


		/* Output all data into a std::string. */
		std::string ToString() const

		
		/* Dump the data into console / debug.log. */
		void print() const
	};




	/** Describes a place in the block chain to another node such that if the
	 * other node doesn't have the same branch, it can find a recent common trunk.
	 * The further back it is, the further before the fork it may be.
	 */
	class CBlockLocator
	{
	protected:
		std::vector<uint1024> vHave;
		
	public:
		
		IMPLEMENT_SERIALIZE
		(
			if (!(nType & SER_GETHASH))
				READWRITE(nVersion);
			READWRITE(vHave);
		)

		
		/* Generic Constructor. */
		CBlockLocator();

		
		/* Set by Block Index. */
		explicit CBlockLocator(const CBlockIndex* pindex);
		
		
		/* Set by single block hash. */
		explicit CBlockLocator(uint1024 hashBlock);
		
		
		/* Set by List of Vectors. */
		CBlockLocator(const std::vector<uint1024>& vHaveIn) { vHave = vHaveIn; }

		
		/* Set the State of Object to NULL. */
		void SetNull();
		
		
		/* Check the State of Object as NULL. */
		bool IsNull();
		
		
		/* Set from Block Index Object. */
		void Set(const CBlockIndex* pindex);
		
		
		/* Find the total blocks back locater determined. */
		int GetDistanceBack();
		
		
		/* Get the Index object stored in Locator. */
		CBlockIndex* GetBlockIndex();
		
		
		/* Get the hash of block. */
		uint1024 GetBlockHash();
		
		
		/* Get the Height of the Locator. */
		int GetHeight();
		
	};
	
		
		
	
	/* __________________________________________________ (Block Processing Methods) __________________________________________________  */
	
	
	
	
	
	
	/* Find the Nearest Orphan Block down the Chain. */
	uint1024 GetOrphanRoot(const CBlock* pblock);
	
	
	/* Find the blocks that are required to complete an orphan chain. */
	uint1024 WantedByOrphan(const CBlock* pblockOrphan);
	
	
	/* Get Coinbase Reweards for Given Blocks. */
	int64 GetProofOfWorkReward(unsigned int nBits);
	
	
	/* Get the Stake Reward for Given Blocks. */
	int64 GetProofOfStakeReward(int64 nCoinAge);
	
	
	/* Search back for an index given PoW / PoS parameters. */
	const CBlockIndex* GetLastBlockIndex(const CBlockIndex* pindex, bool fProofOfStake);
	
	
	/* Search back for an index of given Mining Channel. */
	const CBlockIndex* GetLastChannelIndex(const CBlockIndex* pindex, int nChannel);
	
	
	/* Calculate the majority of blocks that other peers have. */
	int GetNumBlocksOfPeers();
	
	
	/* Determine if the node is syncing from scratch. */
	bool IsInitialBlockDownload();
	
	
	/* Accept a block into the block chain without setting it as the leading block. */
	bool AcceptBlock(LLP::CNode* pfrom, CBlock* pblock);
	
	
	/* Add a block into index memory and give it a location in the chain. */
	bool AddToBlockIndex(LLP::CNode* pfrom, CBlock* pblock);
	
	
	/* Set block as the current leading block of the block chain. */
	bool SetBestChain(LLP::CNode* pfrom, CBlock* pblock);
	
	
	/* Check the disk space for the current partition database is stored in. */
	bool CheckDiskSpace(uint64 nAdditionalBytes = 0);
	
	
	/* Read the block from file and binary postiion (blk0001.dat), */
	FILE* OpenBlockFile(unsigned int nFile, unsigned int nBlockPos, const char* pszMode);
	
	
	/* Add a block to the block file and binary position (blk0001.dat). */
	FILE* AppendBlockFile(unsigned int& nFileRet);
	
	
	/* Load the Genesis and other blocks from the BDB/LLD Indexes. */
	bool LoadBlockIndex(bool fAllowNew = true);
	
	
	/* Ensure that a disk read block index is valid still (prevents needs for checks in data. */
	bool CheckBlockIndex(uint1024 hashBlock);
	
	
	/* Initialize the Mining LLP to start creating work and accepting blocks to broadcast to the network. */
	void StartMiningLLP();
	
	
	/* Create a new block with given parameters and optional coinbase transaction. */
	CBlock* CreateNewBlock(Wallet::CReserveKey& reservekey, Wallet::CWallet* pwallet, unsigned int nChannel, unsigned int nID = 1, LLP::Coinbase* pCoinbase = NULL);
	
	
	/* Add the Transactions to a Block from the Memroy Pool (TODO: Decide whether to put this in transactions.h/transactions.cpp or leave it here). */
	void AddTransactions(std::vector<CTransaction>& vtx, CBlockIndex* pindexPrev);
	
	
	/* Check that the Proof of work is valid for the given Mined block before sending it into the processing queue. */
	bool CheckWork(CBlock* pblock, Wallet::CWallet& wallet, Wallet::CReserveKey& reservekey);
	

	/* TODO: Remove this where not needed. */
	std::string GetChannelName(int nChannel);
	

}


#endif
