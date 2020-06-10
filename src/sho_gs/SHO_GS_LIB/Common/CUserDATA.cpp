#include "stdAFX.h"
#include "IO_STB.h"
#include "CUserDATA.h"
#include "Object.h"
#include "Calculation.h"

#define	MAX_INT		0x07fffffff

short tagBankData::Get_EmptySlot (short nStartSlotNO, short nEndSlotNO)
{
	for (short nI=nStartSlotNO; nI<nEndSlotNO; nI++) 
		if ( 0 == this->m_ItemLIST[ nI ].GetHEADER() )
			return nI;

	return -1;
}

short tagBankData::Add_ITEM(tagITEM &sITEM, short nStartSlotNO, short nEndSlotNO)
{
	if ( 0 == sITEM.GetTYPE() || sITEM.GetTYPE() >= ITEM_TYPE_MONEY ) {
		return -1;
	}

	short nI;
	if ( sITEM.GetTYPE() >= ITEM_TYPE_USE ) {
		// 중복 될수 있는 아이템이다.
		for (nI=0; nI<nEndSlotNO; nI++) {
			if ( this->m_ItemLIST[ nI ].GetHEADER() != sITEM.GetHEADER() )
				continue;
			
			if ( this->m_ItemLIST[ nI ].GetQuantity() + sITEM.GetQuantity() <= MAX_DUP_ITEM_QUANTITY ) {
				this->m_ItemLIST[ nI ].m_uiQuantity += sITEM.GetQuantity();
				return nI;
			}
		}
	}
	
	nI = this->Get_EmptySlot( nStartSlotNO, nEndSlotNO );
	if ( nI >= nStartSlotNO ) {
		this->m_ItemLIST[ nI ] = sITEM;
		return nI;
	}

	return -1;
}

short tagBankData::Add_ITEM(short nSlotNO, tagITEM &sITEM)
{
	if ( sITEM.GetTYPE() >= ITEM_TYPE_USE ) {
		// 중복 될수 있는 아이템이다.
		this->m_ItemLIST[ nSlotNO ].m_uiQuantity += sITEM.GetQuantity();
		return nSlotNO;
	}

	this->m_ItemLIST[ nSlotNO ] = sITEM;
	if ( this->m_ItemLIST[ nSlotNO ].GetQuantity() > MAX_DUP_ITEM_QUANTITY )
		this->m_ItemLIST[ nSlotNO ].m_uiQuantity = MAX_DUP_ITEM_QUANTITY;

	return nSlotNO;
}

short tagBankData::Sub_ITEM(short nSlotNO, tagITEM &sITEM)
{
	if ( 0 == sITEM.GetTYPE() || sITEM.GetTYPE() >= ITEM_TYPE_MONEY ) {
		return -1;
	}

	if ( sITEM.GetTYPE() >= ITEM_TYPE_USE ) {
		// 중복 될수 있는 아이템이다.

		if ( this->m_ItemLIST[ nSlotNO ].GetQuantity() > sITEM.GetQuantity() ) {
			this->m_ItemLIST[ nSlotNO ].m_uiQuantity -= sITEM.GetQuantity();
			return nSlotNO;
		}

		sITEM.m_uiQuantity = this->m_ItemLIST[ nSlotNO ].GetQuantity();
	}

	// 슬롯 비움.
	this->m_ItemLIST[ nSlotNO ].Clear();

	return nSlotNO;
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void CUserDATA::Cal_AddAbility (tagITEM &sITEM, short nItemTYPE)
{
	if ( sITEM.GetItemNO() < 1 || sITEM.GetLife() < 1 )		/// 아이템이 없거나 수명이 다한것은 통과~
		return;

	short nI, nC, nType, nValue;

	// 옵션/박힌 보석에 대해서...
	if ( sITEM.GetGemNO() && ( sITEM.IsAppraisal() || sITEM.HasSocket() ) ) {
		for (nI=0; nI<2; nI++) {
			nC = sITEM.GetGemNO();
			nType  = GEMITEM_ADD_DATA_TYPE ( nC, nI );
			nValue = GEMITEM_ADD_DATA_VALUE( nC, nI );

			_ASSERT( nType <= AT_MAX );

			this->m_iAddValue[ nType ] += nValue;
		}
	}

	for (nI=0; nI<2; nI++) {
		nType = ITEM_NEED_UNION(nItemTYPE, sITEM.m_nItemNo, nI);
		if ( nType && ( nType != this->GetCur_JOHAP() ) )
			continue;

		nType = ITEM_ADD_DATA_TYPE	(nItemTYPE, sITEM.m_nItemNo, nI);
		nValue= ITEM_ADD_DATA_VALUE	(nItemTYPE, sITEM.m_nItemNo, nI);

		this->m_iAddValue[ nType ] += nValue;
	}
}

/// 아이템에 붙은 추가 능력치 계산.
void CUserDATA::Cal_AddAbility ()
{
	tagITEM	sITEM;

	sITEM = this->Get_EquipITEM( EQUIP_IDX_FACE_ITEM );
	Cal_AddAbility(sITEM, ITEM_TYPE_FACE_ITEM	);

	sITEM = this->Get_EquipITEM( EQUIP_IDX_HELMET );
	Cal_AddAbility(sITEM, ITEM_TYPE_HELMET);

	sITEM = this->Get_EquipITEM( EQUIP_IDX_ARMOR );
	Cal_AddAbility(sITEM, ITEM_TYPE_ARMOR);

	sITEM = this->Get_EquipITEM( EQUIP_IDX_KNAPSACK );
	Cal_AddAbility(sITEM, ITEM_TYPE_KNAPSACK);

	sITEM = this->Get_EquipITEM( EQUIP_IDX_GAUNTLET );
	Cal_AddAbility(sITEM, ITEM_TYPE_GAUNTLET);

	sITEM = this->Get_EquipITEM( EQUIP_IDX_BOOTS );
	Cal_AddAbility(sITEM, ITEM_TYPE_BOOTS	);

	sITEM = this->Get_EquipITEM( EQUIP_IDX_WEAPON_R );
	Cal_AddAbility(sITEM, ITEM_TYPE_WEAPON	);

	sITEM = this->Get_EquipITEM( EQUIP_IDX_WEAPON_L );
	Cal_AddAbility(sITEM, ITEM_TYPE_SUBWPN	);

	sITEM = this->Get_EquipITEM( EQUIP_IDX_NECKLACE );
	Cal_AddAbility(sITEM, ITEM_TYPE_JEWEL	);

	sITEM = this->Get_EquipITEM( EQUIP_IDX_RING );
	Cal_AddAbility(sITEM, ITEM_TYPE_JEWEL	);

	/// 2004/02/21 icarus 추가 : 귀걸이 2번째 능력치 적용안되던사항.
	sITEM = this->Get_EquipITEM( EQUIP_IDX_EARRING );
	Cal_AddAbility(sITEM, ITEM_TYPE_JEWEL	);

	if ( this->GetCur_MOVE_MODE() > MOVE_MODE_RUN ) {
		// 탑승 모드일경우 최대 무게 증가치는 케릭터에서 뽑아서 적용.
		int iCurAddWgt = this->m_iAddValue[ AT_WEIGHT ];
		// 무게빼고 초기화.
		::ZeroMemory (this->m_iAddValue,	sizeof(int) * AT_MONEY );
		this->m_iAddValue[ AT_WEIGHT ] = iCurAddWgt;

		for (short nI=0; nI<MAX_RIDING_PART; nI++) {
			sITEM = this->m_Inventory.m_ItemRIDE[ nI ];
			Cal_AddAbility(sITEM, ITEM_TYPE_RIDE_PART);
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CUserDATA::Cal_RecoverHP ()
{
	m_btRecoverHP = this->GetPassiveSkillValue( AT_PSV_RECOVER_HP ) + (short)( m_iAddValue[ AT_RECOVER_HP  ] * this->GetPassiveSkillRate( AT_PSV_RECOVER_HP ) / 100.f );
}

//-------------------------------------------------------------------------------------------------
void CUserDATA::Cal_BattleAbility ()
{
	int iDefDura = 0;

	tagITEM *pITEM;
	if ( this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN ) { // 타이완이면 내구도는 캐릭터 껄로...
		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_HELMET ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();

		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_ARMOR ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();

		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_KNAPSACK ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();

		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_GAUNTLET ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();

		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_BOOTS ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();

		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_WEAPON_L ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();
	} 
	else if( this->GetCur_MOVE_MODE() > MOVE_MODE_RUN ) {
		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_HELMET ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();

		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_ARMOR ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();

		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_KNAPSACK ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();

		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_GAUNTLET ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();

		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_BOOTS ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();

		pITEM = &this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_WEAPON_L ];
		if ( pITEM->m_wHeader && pITEM->GetLife() )	iDefDura += pITEM->GetDurability();
	}
	m_Battle.m_iDefDurabity = iDefDura;

	::ZeroMemory (this->m_iAddValue,	sizeof(int) * AT_MONEY );
	// 패시브 보정영역 제외..
	::ZeroMemory (&this->m_iAddValue[ AT_AFTER_PASSIVE_SKILL ],		sizeof(int)*(AT_MAX-AT_AFTER_PASSIVE_SKILL) );

	::ZeroMemory (this->m_nPassiveRate,	sizeof(short) * AT_MONEY );
	// 패시브 보정영역 제외..
	::ZeroMemory (&this->m_nPassiveRate[ AT_AFTER_PASSIVE_SKILL ],	sizeof(short)*(AT_MAX-AT_AFTER_PASSIVE_SKILL) );


	// 아이템에 붙은 추가 능력치 계산
	Cal_AddAbility ();		// 항상 앞서도록 !!!

	// 패시브 스킬 추가 능력치 값을 추가 능력치 값에 적용...
	Cal_RecoverHP ();
	Cal_RecoverMP ();
	if ( this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN ) {
		m_iAddValue[ AT_STR	  ] += m_PassiveAbilityFromValue[ AT_STR	-AT_STR ];
		m_iAddValue[ AT_DEX	  ] += m_PassiveAbilityFromValue[ AT_DEX	-AT_STR ];
		m_iAddValue[ AT_INT	  ] += m_PassiveAbilityFromValue[ AT_INT	-AT_STR ];
		m_iAddValue[ AT_CON	  ] += m_PassiveAbilityFromValue[ AT_CON	-AT_STR ];
		m_iAddValue[ AT_CHARM ] += m_PassiveAbilityFromValue[ AT_CHARM	-AT_STR ];
		m_iAddValue[ AT_SENSE ] += m_PassiveAbilityFromValue[ AT_SENSE	-AT_STR ];
	}

	// 추가 능력치 계산이 완료된후 처리 해야됨...
	Cal_MaxHP ();
	Cal_MaxMP ();
	Cal_ATTACK ();
	Cal_HIT ();
	Cal_DEFENCE ();
	Cal_RESIST ();
	Cal_MaxWEIGHT ();
	Cal_AvoidRATE ();	// Cal_DEFENCE보다 뒤에 오게..
	Cal_CRITICAL ();
#ifdef	__APPLY_2ND_JOB
	Cal_IMMUNITY();
#endif

	short nI;
	/// 현재 소지하고 있는 아이템들의 무게를 계산...
	m_Battle.m_nWEIGHT = 0;
	for (nI=EQUIP_IDX_FACE_ITEM; nI<INVENTORY_TOTAL_SIZE; nI++) {
		m_Battle.m_nWEIGHT += m_Inventory.GetWEIGHT( nI );
	}

	Cal_DropRATE ();
	m_Battle.m_fRateUseMP = ( 100 - this->GetCur_SaveMP() ) / 100.f;

	if ( GetCur_HP() > GetCur_MaxHP() ) SetCur_HP( GetCur_MaxHP() );

	if ( this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN ) {
		// 대만 보행모드면...
		int iCurAbility;
		for (nI=0; nI<BA_MAX; nI++) {
			iCurAbility = this->m_BasicAbility.m_nBasicA[ nI ] + m_iAddValue[ AT_STR + nI ];
			m_PassiveAbilityFromRate[ nI ] = (int)( iCurAbility * m_nPassiveRate[ AT_STR+nI ] / 100.f );
		}
	}

#ifdef __KCHS_BATTLECART__
	Cal_PatMaxHP();
#endif

	/// 2차 전직 능력치 적용
	Apply_2ndJob_Ability();
}

//-------------------------------------------------------------------------------------------------
void CUserDATA::Cal_RecoverMP ()
{
	m_btRecoverMP = this->GetPassiveSkillValue( AT_PSV_RECOVER_MP ) + (short)( m_iAddValue[ AT_RECOVER_MP  ] * this->GetPassiveSkillRate( AT_PSV_RECOVER_MP ) / 100.f );
}


#ifdef __KCHS_BATTLECART__
void CUserDATA::Cal_PatMaxHP ()
{
	m_Battle.m_nPatMaxHP = 0;
	for (short nI=0; nI<MAX_RIDING_PART; nI++) {
		tagITEM sITEM = this->m_Inventory.m_ItemRIDE[ nI ];
		m_Battle.m_nPatMaxHP += PAT_ITEM_HP( sITEM.GetItemNO() );		
	}

	if( this->m_GrowAbility.m_nPatHP > m_Battle.m_nPatMaxHP )
		this->m_GrowAbility.m_nPatHP = m_Battle.m_nPatMaxHP;
}
#endif

//-------------------------------------------------------------------------------------------------
int CUserDATA::Cal_MaxHP ()
{
	int iA, iM1, iM2;
	float fC;
	switch( this->GetCur_JOB() ) {
		case CLASS_SOLDIER_111 :	// 솔져(111)		[(LV+5)*SQRT(LV+20)*3.5+(STR*2)] + 아이템 증가치 
									iA=5,	iM1=20, fC=3.5f;	break;
		case CLASS_SOLDIER_121 :	// 나이트(121)		[(LV+5)*SQRT(LV+28)*3.5+(STR*2)] + 아이템 증가치 
									iA=5,	iM1=28,	fC=3.5f;	break;
		case CLASS_SOLDIER_122 :	// 챔프(122			[(LV+5)*SQRT(LV+22)*3.5+(STR*2)] + 아이템 증가치 
									iA=5,	iM1=22,	fC=3.5f;	break;

		case CLASS_MAGICIAN_211 :	// 뮤즈(211)		[(LV+4)*SQRT(LV+26)*2.36+(STR*2)] + 아이템 증가치 
									iA=4,	iM1=26,	fC=2.36f;	break;
		case CLASS_MAGICIAN_221 :	// 매지션(221)		[(LV+5)*SQRT(LV+26)*2.37+(STR*2)] + 아이템 증가치 
									iA=5,	iM1=26,	fC=2.37f;	break;
		case CLASS_MAGICIAN_222 :	// 클레릭(222)		[(LV+7)*SQRT(LV+26)*2.4+(STR*2)] + 아이템 증가치 
									iA=7,	iM1=26,	fC=2.4f;	break;

		case CLASS_MIXER_311	 :	// 호커(311)		[(LV+5)*SQRT(LV+20)*2.7+(STR*2)] + 아이템 증가치
									iA=5,	iM1=20,	fC=2.7f;	break;
		case CLASS_MIXER_321	 :	// 레이더(321)		[(LV+5)*SQRT(LV+23)*3+(STR*2)] + 아이템 증가치
									iA=5,	iM1=23,	fC=3.f;		break;
		case CLASS_MIXER_322	 :	// 스카우트(322)	[(LV+5)*SQRT(LV+21)*2.7+(STR*2)] + 아이템 증가치
									iA=5,	iM1=21,	fC=2.7f;	break;

		case CLASS_MERCHANT_411 :	// 딜러(411)		[(LV+5)*SQRT(LV+20)*2.7+(STR*2)] + 아이템 증가치
									iA=5,	iM1=20,	fC=2.7f;	break;
		case CLASS_MERCHANT_421 :	// 부루즈아(421)	[(LV+5)*SQRT(LV+20)*2.7+(STR*2)] + 아이템 증가치
									iA=5,	iM1=20,	fC=2.7f;	break;
		case CLASS_MERCHANT_422 :	// 아티잔(422)		[(LV+5)*SQRT(LV+20)*2.7+(STR*2)] + 아이템 증가치
									iA=5,	iM1=20,	fC=2.7f;	break;

		//case CLASS_VISITOR :		// 무직	비지터(00)	[(LV+4)*SQRT(LV+26)*2.36+(STR*2)] + 아이템 증가치
		default :					iA=4,	iM1=26,	fC=2.36f;	break;
	}

	m_Battle.m_nMaxHP  = (short)( ( this->GetCur_LEVEL()+iA ) * sqrtf(this->GetCur_LEVEL()+iM1) * fC + ( this->GetCur_STR()*2 ) + this->m_iAddValue[ AT_MAX_HP ] );
	
	iA = this->GetPassiveSkillValue( AT_PSV_MAX_HP ) + (short)( m_Battle.m_nMaxHP * this->GetPassiveSkillRate( AT_PSV_MAX_HP ) / 100.f );
	m_Battle.m_nMaxHP += iA;
	this->Cal_AruaMaxHP ();

	return m_Battle.m_nMaxHP;
}

//-------------------------------------------------------------------------------------------------
int CUserDATA::Cal_MaxMP ()
{
	int   iA, iM2;
	float fM1;

	switch( this->GetCur_JOB() ) {
		case CLASS_SOLDIER_111 :	iA=3,	fM1=4.0f,	iM2=4;	break;
		case CLASS_SOLDIER_121 :	iA=0,	fM1=4.5f,	iM2=4;	break;
		case CLASS_SOLDIER_122 :	iA=-6,	fM1=5.0f,	iM2=4;	break;

		case CLASS_MAGICIAN_211 :	iA=0,	fM1=6.0f,	iM2=4;	break;
		case CLASS_MAGICIAN_221 :	iA=-7,	fM1=7.0f,	iM2=4;	break;
		case CLASS_MAGICIAN_222 :	iA=-4,	fM1=6.5f,	iM2=4;	break;

		case CLASS_MIXER_311	 :	iA=4,	fM1=4.0f,	iM2=4;	break;
		case CLASS_MIXER_321	 :	iA=4,	fM1=4.0f,	iM2=4;	break;
		case CLASS_MIXER_322	 :	iA=0,	fM1=4.5f,	iM2=4;	break;

		case CLASS_MERCHANT_411 :	iA=3,	fM1=4.0f,	iM2=4;	break;
		case CLASS_MERCHANT_421 :	iA=3,	fM1=4.0f,	iM2=4;	break;
		case CLASS_MERCHANT_422 :	iA=0,	fM1=4.5f,	iM2=4;	break;

		//case CLASS_VISITOR :
		default :					iA=4,	fM1=3.0f,	iM2=4;	break;
	}

	m_Battle.m_nMaxMP  = ( short) ( ( this->GetCur_LEVEL()+iA ) * fM1 + ( this->GetCur_INT()*iM2 ) ) + this->m_iAddValue[ AT_MAX_MP ];
	iA = this->GetPassiveSkillValue( AT_PSV_MAX_MP ) + (short)( m_Battle.m_nMaxMP * this->GetPassiveSkillRate( AT_PSV_MAX_MP ) / 100.f );
	m_Battle.m_nMaxMP += iA;
	this->Cal_AruaMaxHP ();

	return m_Battle.m_nMaxMP;
}


//-------------------------------------------------------------------------------------------------
int CUserDATA::Cal_MaxWEIGHT ()
{
	// * ITEM = 800 + (LV * 4) + (STR * 6) + 스킬 소지량
	this->m_Battle.m_nMaxWEIGHT  = (int) ( 1100 + ( this->GetCur_LEVEL() * 5 ) + ( this->GetCur_STR() * 6 ) );
	this->m_Battle.m_nMaxWEIGHT += this->m_iAddValue[ AT_WEIGHT ];

	tagITEM *pITEM = &m_Inventory.m_ItemEQUIP[ EQUIP_IDX_KNAPSACK ];
	if ( pITEM->GetHEADER() && pITEM->GetLife() && ITEM_TYPE( pITEM->GetTYPE(), pITEM->GetItemNO() ) == 162 ) {
		// 등짐 배낭.
		short nW = this->GetPassiveSkillValue( AT_PSV_WEIGHT ) + (short)( this->m_Battle.m_nMaxWEIGHT * this->GetPassiveSkillRate( AT_PSV_WEIGHT ) / 100.f );
		this->m_Battle.m_nMaxWEIGHT += nW;
	}

	return this->m_Battle.m_nMaxWEIGHT;
}


//-------------------------------------------------------------------------------------------------
int CUserDATA::Cal_AvoidRATE ()
{
	m_Battle.m_nAVOID = (short)( ( GetCur_DEX()*1.9f + GetCur_LEVEL()*0.3f + 10 ) * 0.4f ) + 
		(short)( this->GetTot_DEF_DURABITY()*0.3f ) + this->GetTot_DEF_GRADE();
	m_Battle.m_nAVOID += this->m_iAddValue[ AT_AVOID ];

	int iPsv = this->GetPassiveSkillValue( AT_PSV_AVOID ) + (short)( this->m_Battle.m_nAVOID * this->GetPassiveSkillRate( AT_PSV_AVOID ) / 100.f );
	this->m_Battle.m_nAVOID += iPsv;

	this->Cal_AruaAVOID();

	return m_Battle.m_nAVOID;
}

//-------------------------------------------------------------------------------------------------
int CUserDATA::Cal_CRITICAL ()
{

	if ( this->GetCur_MOVE_MODE() > MOVE_MODE_RUN) {
		m_Battle.m_iCritical  = (int)( GetCur_SENSE() + ( GetCur_CON() + 20 ) * 0.2f );
	} else {
		m_Battle.m_iCritical  = (int)( GetCur_SENSE() + ( GetCur_CON() + 20 ) * 0.2f );
	}
	m_Battle.m_iCritical += this->m_iAddValue[ AT_CRITICAL ];

	int iPsv = this->GetPassiveSkillValue( AT_PSV_CRITICAL ) + (short)( this->m_Battle.m_iCritical * this->GetPassiveSkillRate( AT_PSV_CRITICAL ) / 100.f );
	this->m_Battle.m_iCritical += iPsv;

	this->Cal_AruaCRITICAL ();

	return m_Battle.m_iCritical;
}

int CUserDATA::Cal_RESIST ()
{
	int iTotRES=0, iTotGradeRES=0;

	for (short nE=EQUIP_IDX_NULL+1; nE<MAX_EQUIP_IDX; nE++) {
		if ( m_Inventory.m_ItemEQUIP[ nE ].GetLife() < 1 || 0 == m_Inventory.m_ItemEQUIP[ nE ].GetTYPE() )
			continue;

		if ( ITEM_RESISTENCE( m_Inventory.m_ItemEQUIP[ nE ].GetTYPE(), m_Inventory.m_ItemEQUIP[ nE ].GetItemNO() ) ) {
			iTotRES		 += ITEM_RESISTENCE( m_Inventory.m_ItemEQUIP[ nE ].GetTYPE(), m_Inventory.m_ItemEQUIP[ nE ].GetItemNO() );
			iTotGradeRES += ITEMGRADE_RES( m_Inventory.m_ItemEQUIP[ nE ].GetGrade() );
		}
	}

	this->m_Battle.m_nRES = (int) ( iTotRES + (iTotGradeRES) + ( this->GetCur_INT()+5 ) * 0.6f + (this->GetCur_LEVEL()+15) * 0.8f );
	this->m_Battle.m_nRES += this->m_iAddValue[ AT_RES ];

	iTotRES = this->GetPassiveSkillValue( AT_PSV_RES ) + (short)( this->m_Battle.m_nRES * this->GetPassiveSkillRate( AT_PSV_RES ) / 100.f );
	this->m_Battle.m_nRES += iTotRES;

	return this->m_Battle.m_nRES;
}

int CUserDATA::Cal_HIT ()
{
	int iHitRate;
	tagITEM*pRightWPN; 

	 if ( this->GetCur_MOVE_MODE() > MOVE_MODE_RUN ) {
		pRightWPN = this->Get_EquipItemPTR( EQUIP_IDX_WEAPON_R );
		if ( pRightWPN->GetItemNO() && pRightWPN->GetLife() > 0 ) {
			iHitRate = (int) ( ( GetCur_CON() + 10 )*0.8f ) + 
					   (int) ( ( ITEM_QUALITY(ITEM_TYPE_WEAPON,pRightWPN->GetItemNO()) ) * 0.6f + ITEMGRADE_HIT(pRightWPN->GetGrade()) + pRightWPN->GetDurability()*0.8f );
		} else {
			// 맨손
			iHitRate = (int)( ( GetCur_CON() + 10 )*0.5f + 15 );
		}
	} else {
		pRightWPN = &this->m_Inventory.m_ItemRIDE[ RIDE_PART_ARMS ];
		if ( pRightWPN->GetLife() > 0 )
			iHitRate = (int) ( ( GetCur_CON()+10 ) * 0.8f ) + ( GetCur_LEVEL() * 0.5f ) +
					   (int) ( ITEM_QUALITY( ITEM_TYPE_RIDE_PART, pRightWPN->GetItemNO() ) * 1.2f );
		else
			iHitRate = 0;
	}

	this->m_Battle.m_nHIT = iHitRate + this->m_iAddValue[ AT_HIT ];

	iHitRate = this->GetPassiveSkillValue( AT_PSV_HIT ) + (short)( this->m_Battle.m_nHIT * this->GetPassiveSkillRate( AT_PSV_HIT ) / 100.f );
	this->m_Battle.m_nHIT += iHitRate;

	this->Cal_AruaHIT ();
	return this->m_Battle.m_nHIT;
}

#ifdef	__APPLY_2ND_JOB
// kchs : 2005-08-20 , 면역력 추가, 그리고 패시브 능력치 적용
int CUserDATA::Cal_IMMUNITY ()
{
	this->m_Battle.m_nImmunity += (short)this->GetPassiveSkillValue( AT_PSV_IMMUNITY ) + (short)( this->m_Battle.m_nImmunity * this->GetPassiveSkillRate( AT_PSV_IMMUNITY ) / 100.f );	 
	return this->m_Battle.m_nImmunity;
}
#endif

//-------------------------------------------------------------------------------------------------
int CUserDATA::Cal_DEFENCE ()
{
	int iTotDEF=0, iTotGradeDEF=0, iTotGRADE=0;

	// 모든 아이템에 방어력이 입력될수 있다..
	short nE;
	for (nE=EQUIP_IDX_NULL+1; nE<MAX_EQUIP_IDX; nE++) {
		if ( 0 == m_Inventory.m_ItemEQUIP[ nE ].GetLife() || 0 == m_Inventory.m_ItemEQUIP[ nE ].GetTYPE() )
			continue;
		
		if ( ITEM_DEFENCE( m_Inventory.m_ItemEQUIP[ nE ].GetTYPE(), m_Inventory.m_ItemEQUIP[ nE ].GetItemNO() ) ) {
			iTotDEF		 += ITEM_DEFENCE( m_Inventory.m_ItemEQUIP[ nE ].GetTYPE(), m_Inventory.m_ItemEQUIP[ nE ].GetItemNO() );
			iTotGradeDEF += ITEMGRADE_DEF( m_Inventory.m_ItemEQUIP[ nE ].GetGrade() );
			iTotGRADE	 += m_Inventory.m_ItemEQUIP[ nE ].GetGrade();
		}
	}

	if ( this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN ) {
		this->m_Battle.m_iDefGrade = iTotGRADE;

		this->m_Battle.m_nDEF = (int)(( iTotDEF + (iTotGradeDEF) + ( this->GetCur_STR()+5 ) * 0.35f + (this->GetCur_LEVEL()+15) * 0.7f ) ) + this->m_iAddValue[ AT_DEF ];
	} else {
		for (nE=0; nE<MAX_RIDING_PART; nE++) {
			if ( 0 == m_Inventory.m_ItemRIDE[ nE ].GetLife() || 0 == m_Inventory.m_ItemRIDE[ nE ].GetTYPE() )
				continue;
			
			iTotGradeDEF += ITEMGRADE_DEF( m_Inventory.m_ItemRIDE[ nE ].GetGrade() );
			iTotGRADE	 += m_Inventory.m_ItemRIDE[ nE ].GetGrade();
		}
		this->m_Battle.m_iDefGrade = iTotGRADE;

		this->m_Battle.m_nDEF = (int)(( iTotDEF + (iTotGradeDEF) + ( this->GetCur_STR()+5 ) * 0.35f + (this->GetCur_LEVEL()+15) * 0.7f )*0.8f) + this->m_iAddValue[ AT_DEF ];
	}

	nE = this->GetPassiveSkillValue( AT_PSV_DEF_POW ) + (short)( this->m_Battle.m_nDEF * this->GetPassiveSkillRate( AT_PSV_DEF_POW ) / 100.f );
	this->m_Battle.m_nDEF += nE;

	//	2004. 7. 22 방패착용시면 적용되면 방어력 패시브 스킬을 무조건 적용으로 수정...
	//  2005. 8. 19 방패 패시브 스킬 추가...
	tagITEM *pITEM = &m_Inventory.m_ItemEQUIP[ EQUIP_IDX_WEAPON_L ];
	if ( pITEM->GetHEADER() && pITEM->GetLife() && ITEM_TYPE( pITEM->GetTYPE(), pITEM->GetItemNO() ) == 261 )  {
		// 방패.
		int iPsv = this->GetPassiveSkillValue( AT_PSV_SHIELD_DEF ) + (short)( this->m_Battle.m_nDEF * this->GetPassiveSkillRate( AT_PSV_SHIELD_DEF ) / 100.f );
		this->m_Battle.m_nDEF += iPsv;
	}

	return this->m_Battle.m_nDEF;
}


//-------------------------------------------------------------------------------------------------
int CUserDATA::Cal_ATTACK ()
{
	int iAP = 0;

	tagITEM	*pRightWPN = this->Get_EquipItemPTR( EQUIP_IDX_WEAPON_R );
	int iWeaponAP;

	if ( pRightWPN->GetHEADER() && pRightWPN->GetLife() > 0 ) 
		iWeaponAP = WEAPON_ATTACK_POWER( pRightWPN->m_nItemNo );
	else
		iWeaponAP = 0;		/// 수명이 다한 무기는 무기의 기본 공격력을 0 으로..

	if ( this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN ) {		
		t_eSHOT ShotTYPE = pRightWPN->GetShotTYPE();
		if ( ShotTYPE < MAX_SHOT_TYPE ) {
			tagITEM *pShotITEM = &this->m_Inventory.m_ItemSHOT[ ShotTYPE ];

			short nItemQ, nItemW;
			if ( pShotITEM->IsEtcITEM() ) {
				nItemQ = ITEM_QUALITY(pShotITEM->GetTYPE(), pShotITEM->GetItemNO() );
				nItemW = ITEM_WEIGHT (pShotITEM->GetTYPE(), pShotITEM->GetItemNO() );
			} else {
				nItemQ = nItemW = 0;
			}

			// 소모탄에 따른 공격력 계산...
			switch( ShotTYPE ) {
				case SHOT_TYPE_ARROW :
					iAP = (int)( ( GetCur_DEX()*0.52f + GetCur_STR()*0.1f + GetCur_LEVEL()*0.1f + nItemQ*0.5f ) +
								( ( iWeaponAP+ ITEMGRADE_ATK(pRightWPN->GetGrade()) ) * ( GetCur_DEX()*0.04f + GetCur_SENSE()*0.03f+29 ) / 30 ) ) ;
					break;
				case SHOT_TYPE_BULLET:
					iAP = (int)( ( GetCur_DEX()*0.3f + GetCur_CON()*0.47f + GetCur_LEVEL()*0.1f + nItemQ * 0.8f) +
					( ( (iWeaponAP+ITEMGRADE_ATK(pRightWPN->GetGrade()) ) ) * ( GetCur_CON()*0.04f + GetCur_SENSE()*0.05f+29) / 30 ) );
					break;

				case SHOT_TYPE_THROW :
					iAP = (int)( ( GetCur_STR()*0.32f + GetCur_CON()*0.45f + GetCur_LEVEL()*0.1f + nItemQ * 0.8f ) +
					( ( (iWeaponAP+ITEMGRADE_ATK(pRightWPN->GetGrade()) ) ) * ( GetCur_CON()*0.04f + GetCur_SENSE()*0.05f+29) / 30 ) );
			} 
		} else {
			switch ( WEAPON_TYPE( pRightWPN->m_nItemNo ) / 10 ) {
				case 21 :	// 한손
				case 22 :	// 양손		// 근접 무기
					iAP = (int)( ( GetCur_STR()*0.75f + GetCur_LEVEL()*0.2f ) + ( (iWeaponAP+ITEMGRADE_ATK(pRightWPN->GetGrade()) ) * ( GetCur_STR()*0.05f+29) / 30.f ) );
					break;
				case 24 :				// 마법 무기
					if ( 241 == WEAPON_TYPE( pRightWPN->m_nItemNo ) ) {	
						// 마법 지팡이.
						iAP = (int)( ( GetCur_STR()*0.4f + GetCur_INT()*0.4f + GetCur_LEVEL()*0.2f ) + ( (iWeaponAP+ITEMGRADE_ATK(pRightWPN->GetGrade()) ) * ( GetCur_INT()*0.05f + 29 ) / 30.f ) );
					} else {
						iAP = (int)( ( GetCur_INT()*0.6f + GetCur_LEVEL()*0.2f ) + ( (iWeaponAP+ITEMGRADE_ATK(pRightWPN->GetGrade()) ) * ( GetCur_SENSE()*0.1f + 26 ) / 27.f ) );
					}
					break;
				case 25 :				// 카타르
					if ( 252 == WEAPON_TYPE( pRightWPN->m_nItemNo ) ) {	
						// 이도류
						iAP = (int)( ( GetCur_STR()*0.63f + GetCur_DEX()*0.45f + GetCur_LEVEL()*0.2f ) + ( (iWeaponAP+ITEMGRADE_ATK(pRightWPN->GetGrade()) ) * ( GetCur_DEX()*0.05f + 25 ) / 26.f ) );
					} else {
						iAP = (int)( ( GetCur_STR()*0.42f + GetCur_DEX()*0.55f + GetCur_LEVEL()*0.2f ) + ( (iWeaponAP+ITEMGRADE_ATK(pRightWPN->GetGrade()) ) * ( GetCur_DEX()*0.05f + 20 ) / 21.f ) );
					}
					break;
				case 0  :				// 스킬맨손 공격력 : 무도가 직업에 특한된 스킬로, 패시브 성격의 맨손 공격력을 제공한다.
					iAP = (int)( GetCur_STR()*0.5f + GetCur_DEX()*0.3f + GetCur_LEVEL()*0.2f );
					break;
			}
		}

		// this->m_nPassiveAttackSpeed = this->GetPassiveSkillAttackSpeed( pRightWPN->m_nItemNo );

		iAP += this->m_iAddValue[ AT_ATK ];
		this->m_Battle.m_nATT = iAP + this->GetPassiveSkillAttackPower( iAP, pRightWPN->m_nItemNo );
	} 
	else // PAT 탑승 모드
	{
		t_eSHOT ShotTYPE = pRightWPN->GetShotTYPE();
		/// 수명이 다한 무기는 무기의 기본 공격력을 0 으로..
		if ( pRightWPN->GetHEADER() && pRightWPN->GetLife() > 0 ) 
			iWeaponAP = WEAPON_ATTACK_POWER( pRightWPN->m_nItemNo );
		else
			iWeaponAP = 0;
		int iWeaponTERM = iWeaponAP + ITEMGRADE_ATK(pRightWPN->GetGrade()) + PAT_ITEM_ATK_POW(this->m_Inventory.m_ItemRIDE[ RIDE_PART_ARMS ].GetItemNO());

		if( ShotTYPE < MAX_SHOT_TYPE ) { // 타이완 원거리 무기
			switch( ShotTYPE )
			{
			case SHOT_TYPE_ARROW :
				iAP = ( ( GetCur_DEX()*0.52f + GetCur_STR()*0.1f + GetCur_LEVEL()*0.1f + 3 ) + 
					iWeaponTERM *
					( GetCur_DEX()*0.04f + GetCur_SENSE()*0.03f + 29 ) / 60.f ) * 1.03f;
				break;
			case SHOT_TYPE_BULLET:
				iAP = ( (GetCur_DEX()*0.3f+GetCur_CON()*0.47+GetCur_LEVEL()*0.1f+6.4f) + iWeaponTERM * 
					( GetCur_CON()*0.04f+GetCur_SENSE()*0.05f+29) / 60.f ) * 1.032f;
				break;

			case SHOT_TYPE_THROW :
				iAP = ( (GetCur_STR()*0.32f + GetCur_CON()*0.45f+GetCur_LEVEL()*0.1f + 8) + iWeaponTERM *
					(GetCur_CON() * 0.04f + GetCur_SENSE() * 0.05f + 29) / 60.f ) * 1.033f ;
				break;
			}
		} 
		else // 타이완 근거리 무기
		{
			switch ( WEAPON_TYPE( pRightWPN->m_nItemNo ) / 10 ) {
			case 21 :	// 한손
			case 22 :	// 양손		// 근접 무기
				iAP = ( (GetCur_STR()*0.75f + GetCur_LEVEL() * 0.2f) + iWeaponTERM *( GetCur_STR() * 0.05f + 29) / 60.f ) * 1.033f;
				break;
			case 24 :				// 마법 무기
				{
					if( WEAPON_TYPE( pRightWPN->m_nItemNo ) == 241) {
						// 마법 지팡이
						iAP = ( (GetCur_STR()*0.4f + GetCur_INT()*0.4f + GetCur_LEVEL() * 0.2f) + iWeaponTERM *
							(GetCur_INT()*0.05f + 29) / 60.f ) * 1.03f;
						break;
					} else {	// 마법도구
						iAP = ( (GetCur_INT()*0.6f + GetCur_LEVEL() * 0.2f ) + iWeaponTERM * ( GetCur_SENSE() * 0.1f + 26 )/ 54.f ) * 1.025f;
					}
				}
				break;
			case 25 :				
				if ( 252 == WEAPON_TYPE( pRightWPN->m_nItemNo ) ) {	 // 이도류
					iAP = ( (GetCur_STR() * 0.63f + GetCur_DEX() * 0.45f + GetCur_LEVEL() * 0.2f ) + iWeaponTERM  * 
						(GetCur_DEX() * 0.05f + 25 ) / 52.f ) * 1.032f;
				} else { // 카타르
					iAP = ( (GetCur_STR() * 0.42f + GetCur_DEX() * 0.55f + GetCur_LEVEL() * 0.2f ) + iWeaponTERM * 
						(GetCur_DEX() * 0.05f + 20 ) / 42.f ) * 1.032f;
				}
				break;
			case 0  :
				// [(STR*0.5 + DEX*0.3 + LV*0.2) + (카트무기공격력)]/2+ 아이템 증가치
				iAP = GetCur_STR() * 0.5f + GetCur_DEX() * 0.3f + GetCur_LEVEL() * 0.2f + 
						PAT_ITEM_ATK_POW(this->m_Inventory.m_ItemRIDE[ RIDE_PART_ARMS ].GetItemNO()) / 2.f ;
				break;
			}
		}
		iAP += this->m_iAddValue[ AT_ATK ];
		this->m_Battle.m_nATT = iAP + this->GetPassiveSkillAttackPower( iAP, pRightWPN->m_nItemNo );
	}

	this->Cal_AruaATTACK();

	return this->m_Battle.m_nATT;
}

//-------------------------------------------------------------------------------------------------
float CUserDATA::Cal_RunSPEED ()
{
/*
	* 뛸 때의 이동 속도. (걷기는 디폴트 속도로 모든 아바타가 같다)
	* 장착되어 있는 방어구나 무기의 무게의 합이 높을수록 이동속도가 저하된다.
	* DEX가 높을수록, 장착된 신발의 이동력이 높을수록 이동속도가 높아진다.
	* SPE = { (신발의 이동력 + 15) * (DEX+400) - (장착된 무게+50)*4 } / 70
*/
	float fMoveSpeed;
	if ( this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN ) {
		short nItemSpeed, nItemNo;

		//nItemNo = this->GetCur_PartITEM(BODY_PART_BOOTS);
		nItemNo = this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_BOOTS ].GetItemNO();
		if ( nItemNo > 0 && this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_BOOTS ].GetLife() ) {
			nItemSpeed = BOOTS_MOVE_SPEED( nItemNo );
		} else {
			nItemSpeed = BOOTS_MOVE_SPEED( 0 );			/// 수명이 다한 신발은 맨발 속도로 대체...
		}

		//nItemNo = this->GetCur_PartITEM(BODY_PART_KNAPSACK);
		nItemNo = this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_KNAPSACK ].GetItemNO();
		if ( nItemNo > 0 && this->m_Inventory.m_ItemEQUIP[ EQUIP_IDX_KNAPSACK ].GetLife() ) {
			nItemSpeed += BACKITEM_MOVE_SPEED( nItemNo );
		}
		nItemSpeed += 20;
		fMoveSpeed = nItemSpeed * ( GetCur_DEX() + 500.f ) / 100.f + this->m_iAddValue[ AT_SPEED ];

		float fPsvSpd = GetPassiveSkillValue( AT_PSV_MOV_SPD ) + fMoveSpeed * GetPassiveSkillRate( AT_PSV_MOV_SPD )/100.f;
		return ( fMoveSpeed + fPsvSpd );
	} else {
		tagITEM *pLEG = &this->m_Inventory.m_ItemRIDE[ RIDE_PART_LEG	];
		tagITEM *pENG = &this->m_Inventory.m_ItemRIDE[ RIDE_PART_ENGINE	];

		if ( pLEG->GetLife() && pENG->GetLife() )
			fMoveSpeed = PAT_ITEM_MOV_SPD( pLEG->GetItemNO() ) * PAT_ITEM_MOV_SPD( pENG->GetItemNO() ) / 10.f;
		else
			fMoveSpeed = 200;	/// 이동 최소 속도

		fMoveSpeed += this->m_iAddValue[ AT_SPEED ];
		if ( this->Get_WeightRATE() >= WEIGHT_RATE_WALK && fMoveSpeed > 300.f )
			fMoveSpeed = 300.f;
	}
	this->Cal_AruaRunSPD ();
	
	return fMoveSpeed;
}

//-------------------------------------------------------------------------------------------------
// 패시브 스킬에서 참조할 버프전의 능력치...
/*
int CUserDATA::GetOri_AbilityValue (WORD wType)
{
	switch( wType ) {
		// 패시브 스킬 참조시 기본 능력치는 바로 배열로 계산한다.
		//case AT_STR	:		return GetCur_STR ();
		//case AT_DEX	:		return GetCur_DEX ();
		//case AT_INT	:		return GetCur_INT ();
		//case AT_CON	:		return GetCur_CON ();
		//case AT_CHARM	:		return GetCur_CHARM ();
		//case AT_SENSE	:		return GetCur_SENSE ();

		case AT_ATK		:		return GetDef_ATK ();
		case AT_DEF		:		return GetDef_DEF ();
		case AT_HIT		:		return GetDef_HIT ();
		case AT_RES		:		return GetDef_RES ();
		case AT_AVOID	:		return GetDef_AVOID ();
		case AT_SPEED	:		return (int)GetCur_MOVE_SPEED ();
		case AT_ATK_SPD :		return GetDef_ATK_SPD ();
		case AT_WEIGHT	:		return GetDef_WEIGHT ();
		case AT_CRITICAL   :	return GetDef_CRITICAL ();

		case AT_RECOVER_HP :	
		case AT_RECOVER_MP :	break;

		case AT_MAX_HP :		return GetDef_MaxHP ();
		case AT_MAX_MP :		return GetDef_MaxMP ();
	}

	return 0;
}
*/

//-------------------------------------------------------------------------------------------------
bool CUserDATA::Set_AbilityValue (WORD nType, int iValue)
{
	switch( nType ) 
	{
		case AT_SEX			:	SetCur_SEX ( iValue );		return true;
		case AT_FACE		:	SetCur_FACE( iValue );		return true;
		case AT_HAIR		:	SetCur_HAIR( iValue );		return true;

		case AT_CLASS		:	SetCur_JOB( iValue );		return true;	
		/*
		case AT_BIRTHSTONE :		
		case AT_RANK	:			
		case AT_FAME	:			
		case AT_FACE	:
		case AT_HAIR	:			
			break;
		*/
		case AT_UNION		:	SetCur_UNION( iValue );		return true;

		case AT_STR			:	SetDef_STR  ( iValue );		return true;
		case AT_DEX			:	SetDef_DEX  ( iValue );		return true;
		case AT_INT			:	SetDef_INT  ( iValue );		return true;
		case AT_CON			:	SetDef_CON	( iValue );		return true;
		case AT_CHARM		:	SetDef_CHARM( iValue );		return true;
		case AT_SENSE		:	SetDef_SENSE( iValue );		return true;

		case AT_HP			:	SetCur_HP( iValue );		return true;
		case AT_MP			:	SetCur_MP( iValue );		return true;
		case AT_STAMINA		:	SetCur_STAMINA( iValue );	return true;

		/*
		case AT_ATK		:			
		case AT_DEF		:			
		case AT_HIT		:			
		case AT_RES		:			
		case AT_AVOID	:			
		case AT_SPEED	:			
		case AT_ATK_SPD :		
		case AT_WEIGHT	:
		case AT_CRITICAL   :
		case AT_RECOVER_HP :
		case AT_RECOVER_MP :
		*/
		case AT_LEVEL		:	SetCur_LEVEL( (WORD)iValue );	return true;		
		case AT_EXP			:	SetCur_EXP( iValue );			return true;
		/*
		case AT_BONUSPOINT	:
		case AT_SKILLPOINT	:
		case AT_CHAOS	:
		case AT_PK_LEV	:		

		case AT_HEAD_SIZE :		
		case AT_BODY_SIZE :		
		// case AT_BATTLE_LEV :
			break;
		*/

		case AT_PK_FLAG :	SetCur_PK_FLAG( iValue );			return true;
		case AT_TEAM_NO :	SetCur_TeamNO( iValue );			return true;	

		default :
			if ( nType >= AT_UNION_POINT1 && nType <= AT_UNION_POINT10 ) {
				SetCur_JoHapPOINT( nType-AT_UNION_POINT1 + 1, iValue );
			}
	}

	return false;
}

void CUserDATA::Add_AbilityValue (WORD wType, int iValue)
{
	switch( wType ) {
		/*
		case AT_BIRTHSTONE :
		case AT_CLASS	:			
		case AT_UNION	:			
		case AT_RANK	:			
		case AT_FAME	:			

		case AT_FACE	:
		case AT_HAIR	:			
			break;
		*/
		case AT_STR			:	SetDef_STR  ( GetDef_STR()   + iValue );	return;
		case AT_DEX			:	SetDef_DEX  ( GetDef_DEX()   + iValue );	return;
		case AT_INT			:	SetDef_INT  ( GetDef_INT()   + iValue );	return;
		case AT_CON			:	SetDef_CON	( GetDef_CON()   + iValue );	return;
		case AT_CHARM		:	SetDef_CHARM( GetDef_CHARM() + iValue );	return;
		case AT_SENSE		:	SetDef_SENSE( GetDef_SENSE() + iValue );	return;

		case AT_HP			:	AddCur_HP( iValue );						return;
		case AT_MP			:	AddCur_MP( iValue );						return;
		case AT_STAMINA		:	AddCur_STAMINA( iValue );					return;

		case AT_GUILD_SCORE	:	AddClanSCORE( iValue );						return;

		case AT_EXP			:	
			AddCur_EXP(iValue);							
			return;
		case AT_BONUSPOINT	:	AddCur_BonusPOINT(iValue);					return;
		case AT_SKILLPOINT	:	AddCur_SkillPOINT(iValue);					return;

		case AT_MONEY		:	
			if ( iValue < 0 && iValue > GetCur_MONEY() )
				break;
			Add_CurMONEY(iValue);	
			break;

		default :
			if ( wType >= AT_UNION_POINT1 && wType <= AT_UNION_POINT10 ) {
				AddCur_JoHapPOINT( wType-AT_UNION_POINT1 +1, iValue );
			}
	}
}

bool CUserDATA::Check_JobCollection( short nClassStbIDX )
{
	if ( 0 == CLASS_INCLUDE_JOB( nClassStbIDX, 0 ) ) {
		// 첫번째 입력된 값이 없으면 통과...
		return true;
	}

	for (short nI=0; nI<CLASS_INCLUDE_JOB_CNT; nI++) {
		if ( 0 == CLASS_INCLUDE_JOB( nClassStbIDX, nI ) ) {
			// 입력된 값중엔 없었다..
			return false;
		}

		if ( this->GetCur_JOB() == CLASS_INCLUDE_JOB( nClassStbIDX, nI ) )
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool CUserDATA::Check_PatEquipCondition( tagITEM &sITEM, short nEquipIdx )
{
	if( sITEM.GetTYPE() != ITEM_TYPE_RIDE_PART || 0 == sITEM.GetItemNO() )
		return false;

	if ( RIDE_PART_ENGINE != PAT_ITEM_PART_IDX( sITEM.GetItemNO() ) && sITEM.GetLife() < 1 )
		return false;

	if( 0 != PAT_ITEM_NEED_ABILITY_IDX( sITEM.GetItemNO() ) )
	{
		int iValue = GetCur_AbilityValue( PAT_ITEM_NEED_ABILITY_IDX( sITEM.GetItemNO() )  );
		if ( iValue < PAT_ITEM_NEED_ABILITY_VALUE( sITEM.GetItemNO() ) )
			return false;
	}

	// 장착 위치가 맞냐 ??
	if ( nEquipIdx != PAT_ITEM_PART_IDX( sITEM.GetItemNO() ) )
		return false;

#ifdef __KCHS_BATTLECART__
	short nClassStbIDX = PAT_ITEM_EQUIP_REQUIRE_CLASS( sITEM.GetItemNO() );
	if( PAT_ITEM_EQUIP_REQUIRE_CLASS( sITEM.GetItemNO() ))
	{
		for (short nI=0; nI<CLASS_INCLUDE_JOB_CNT; nI++) {
			if ( 0 == CLASS_INCLUDE_JOB( nClassStbIDX, nI ) ) {
				// 입력된 값중엔 없었다..
				return false;
			}

			if ( this->GetCur_JOB() == CLASS_INCLUDE_JOB( nClassStbIDX, nI ) )
				return true;
		}
	}
#endif
	
	return true;
}
//-------------------------------------------------------------------------------------------------
const short s_Equip2ItemTYPE[ MAX_EQUIP_IDX ] = {
	0,						// EQUIP_IDX_NULL
	ITEM_TYPE_FACE_ITEM,	// EQUIP_IDX_FACE_ITEM	1,
	ITEM_TYPE_HELMET,		// EQUIP_IDX_HELMET		2,
	ITEM_TYPE_ARMOR,		// EQUIP_IDX_ARMOR,		3
	ITEM_TYPE_KNAPSACK,		// EQUIP_IDX_KNAPSACK	4,
	ITEM_TYPE_GAUNTLET,		// EQUIP_IDX_GAUNTLET	5,
	ITEM_TYPE_BOOTS,		// EQUIP_IDX_BOOTS,		6
	ITEM_TYPE_WEAPON,		// EQUIP_IDX_WEAPON_R	7,
	ITEM_TYPE_SUBWPN,		// EQUIP_IDX_WEAPON_L	8	
	ITEM_TYPE_JEWEL,		// EQUIP_IDX_NECKLACE	9
	ITEM_TYPE_JEWEL,		// EQUIP_IDX_RING		10
	ITEM_TYPE_JEWEL,		// EQUIP_IDX_EARRING	11
} ;	
bool CUserDATA::Check_EquipCondition (tagITEM &sITEM, short nEquipIdx)
{
	if ( sITEM.GetTYPE() == 0 || sITEM.GetLife() < 1 )	/// 수명이 다한 아이템은 장착 불가~
		return false;

	// 장착 아이템 타입으로 장착 위치가 맞냐???
	if ( sITEM.GetTYPE() != s_Equip2ItemTYPE[ nEquipIdx ] )
		return false;

	if ( ITEM_TYPE_JEWEL == sITEM.GetTYPE() ) {
		// 반지, 목걸이, 귀걸이 -- 반지로 목걸이,귀걸이에 장착하는 해킹 방지...
		switch( ITEM_TYPE( sITEM.GetTYPE(),sITEM.GetItemNO() ) ) {
			case JEWEL_ITEM_RING	:	if ( nEquipIdx != EQUIP_IDX_RING )		return false;
				break;
			case JEWEL_ITEM_NECKLACE:	if ( nEquipIdx != EQUIP_IDX_NECKLACE )	return false;
				break;
			case JEWEL_ITEM_EARRING	:	if ( nEquipIdx != EQUIP_IDX_EARRING	)	return false;
				break;
		}
	}

	/// 체크할 직업이 있는가 ?
	if ( !Check_JobCollection( ITEM_EQUIP_REQUIRE_CLASS( sITEM.m_cType, sITEM.m_nItemNo ) ) ) 
		return false;

	///조합 체크<-2004.3.18.nAvy
	int		iCheckCount = 0;
	bool	bCorrect	= false;
	for( int nl = 0; nl < ITEM_EQUIP_REQUIRE_UNION_CNT; nl++ )
	{
		if( 0 != ITEM_EQUIP_REQUIRE_UNION( sITEM.GetTYPE(), sITEM.GetItemNO(), nl ))
		{
			++iCheckCount;
			if( ITEM_EQUIP_REQUIRE_UNION( sITEM.GetTYPE(), sITEM.GetItemNO(), nl ) == this->GetCur_JOHAP() )
			{
				bCorrect = true;
				break;
			}
		}
	}

	if( iCheckCount && !bCorrect)
		return false;
	///->

	int iValue;
	for( short nI = 0; nI < ITEM_NEED_DATA_CNT; ++nI )
	{
		if ( ITEM_NEED_DATA_TYPE ( sITEM.m_cType, sITEM.m_nItemNo, nI) ) {
			iValue = GetCur_AbilityValue( ITEM_NEED_DATA_TYPE ( sITEM.m_cType, sITEM.m_nItemNo, nI) );
			if ( iValue < ITEM_NEED_DATA_VALUE( sITEM.m_cType, sITEM.m_nItemNo , nI) )
				return false;
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
/// USEITEM_NEED_DATA_TYPE : 어떤 능력치를 비교할것인가?
/// USEITEM_NEED_DATA_VALUE : 필요능력치.
/// USEITEM_ADD_DATA_TYPE : 적용능력
/// USEITEM_ADD_DATA_VALUE : 적용 수치
/*
bool CUserDATA::Use_ITEM (WORD wUseItemNO)
{
#ifndef	__SERVER
	int iValue = GetCur_AbilityValue( USEITEM_NEED_DATA_TYPE(wUseItemNO) );
	if ( iValue < USEITEM_NEED_DATA_VALUE(wUseItemNO) )
		return false;
#endif

	// 현재 무게 감소..
	//m_Battle.m_nWEIGHT -= ITEM_WEIGHT( ITEM_TYPE_USE, wUseItemNO );

	if ( 0 == USEITME_STATUS_STB( wUseItemNO ) ) 
	{
		// 지속형 상태로 소모되는 아이템이 아니면...
		Add_AbilityValue( USEITEM_ADD_DATA_TYPE( wUseItemNO ), USEITEM_ADD_DATA_VALUE( wUseItemNO ) );
	}

	return true;
}
*/

///
/// 몸에 붙어야할 아이템들은 몸에 붙여 준다.
/// 아닌 아이템은 ..?
///
///

bool CUserDATA::Set_EquipITEM (short nEquipIDX, tagITEM &sITEM)
{
	// 비어 있지 않음?
	//if ( m_Inventory.m_ItemEQUIP[ nEquipIDX ].m_cType )
	//	return false;
	//m_Inventory.m_ItemEQUIP[ nEquipIDX ] = sITEM;
	this->Set_ITEM( nEquipIDX, sITEM );
//	m_Inventory.AppendITEM( nEquipIDX, sITEM, m_Battle.m_nWEIGHT );

	switch ( nEquipIDX ) {
		case EQUIP_IDX_FACE_ITEM :
			SetCur_PartITEM( BODY_PART_FACE_ITEM,	sITEM );
			break;
		case EQUIP_IDX_HELMET :
			SetCur_PartITEM( BODY_PART_HELMET,		sITEM );
			break;
		case EQUIP_IDX_ARMOR :
			SetCur_PartITEM( BODY_PART_ARMOR,		sITEM );
			break;
		case EQUIP_IDX_KNAPSACK :
			SetCur_PartITEM( BODY_PART_KNAPSACK,	sITEM );			
			break;		
		case EQUIP_IDX_GAUNTLET :
			SetCur_PartITEM( BODY_PART_GAUNTLET,	sITEM );
			break;
		case EQUIP_IDX_BOOTS :
			SetCur_PartITEM( BODY_PART_BOOTS,		sITEM );
			break;
		case EQUIP_IDX_WEAPON_R :
			SetCur_PartITEM( BODY_PART_WEAPON_R,	sITEM );
			break;
		case EQUIP_IDX_WEAPON_L :
			SetCur_PartITEM( BODY_PART_WEAPON_L,	sITEM );
			break;
	} 

	// 장착 아이템이 바뀌었으니... 해줘야지...
	this->UpdateCur_Ability ();
	this->InitPassiveSkill ();

#ifndef	__SERVER
	/// 모델 데이터 갱신
	UpdateModelData();
#endif

	return true;
}


//-------------------------------------------------------------------------------------------------
// 스킬 사용시 소모될 능력치의 수치를 얻는다.
int CUserDATA::Skill_ToUseAbilityVALUE( short nSkillIDX, short nPropertyIDX )
{
	int iValue = SKILL_USE_VALUE( nSkillIDX, nPropertyIDX );
	if ( AT_MP == SKILL_USE_PROPERTY(nSkillIDX, nPropertyIDX) ) {
		return (int)( iValue * this->GetCur_RateUseMP() );
	}

#ifdef __KCHS_BATTLECART__
	if( AT_PATHP == SKILL_USE_PROPERTY(nSkillIDX, nPropertyIDX) )
	{
		if( GetCur_PatHP() < iValue && GetCur_PatHP() > 0 )
			return GetCur_PatHP();		// __KCHS_BATTLECART__ 이렇게 하지 않으면 카트 체력은 영원히 0이 안 될수 있고. 그래서 쿨타임 절대로 안 돌아간다.
	}
#endif

	return iValue;
}

//-------------------------------------------------------------------------------------------------
bool CUserDATA::Skill_UseAbilityValue( short nSkillIDX )
{
	int iValue;
	for (short nI=0; nI<SKILL_USE_PROPERTY_CNT; nI++) {
		if ( 0 == SKILL_USE_PROPERTY( nSkillIDX, nI ) )
			break;

		iValue = Skill_ToUseAbilityVALUE( nSkillIDX, nI );

		switch( SKILL_USE_PROPERTY( nSkillIDX, nI ) ) {
			case AT_STAMINA :	this->SetCur_STAMINA( GetCur_STAMINA()-iValue );	break;
			case AT_HP		:	this->SubCur_HP( iValue );							break;
			case AT_MP		:	this->SubCur_MP( iValue );							break;
			case AT_EXP		:	this->SetCur_EXP( GetCur_EXP() - iValue );			break;
			case AT_MONEY	:	this->SetCur_MONEY( GetCur_MONEY() - iValue );		break;

			case AT_FUEL	:	this->SubCur_FUEL( iValue );						break;
#ifdef __KCHS_BATTLECART__
			case AT_PATHP	:	this->SubCur_PatHP( iValue );						break;
#endif
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
int CUserDATA::Skill_GetAbilityValue( short nAbilityType )
{
	switch( nAbilityType ) 
	{
		case AT_LEVEL	:		return this->GetCur_LEVEL();

		case AT_STR		:		return GetCur_STR ();
		case AT_DEX		:		return GetCur_DEX ();
		case AT_INT		:		return GetCur_INT ();
		case AT_CON		:		return GetCur_CON ();
		case AT_CHARM	:		return GetCur_CHARM ();
		case AT_SENSE	:		return GetCur_SENSE ();

		case AT_HP		:		return ( GetCur_HP ()-1 );	//// 실제보다 1작게...피인 경우 소모시 다빼면 뒈진다..
		case AT_MP		:		return GetCur_MP ();
		case AT_EXP		:		return GetCur_EXP();
		case AT_MONEY	:		
			if ( GetCur_MONEY () > MAX_INT )
				return MAX_INT;
			return (int)GetCur_MONEY();
		case AT_STAMINA :		return GetCur_STAMINA ();


		case AT_FUEL	:		return GetCur_FUEL ();
		// case AT_SKILLPOINT :	return GetCur_SkillPOINT ();
#ifdef __KCHS_BATTLECART__
		case AT_PATHP	:		return GetCur_PatHP ();
#endif
	}

	return -999;
}

//-------------------------------------------------------------------------------------------------
short CUserDATA::Skill_FindEmptySlot( short nSkillIDX )
{
	short nSlot=-1;
	char cPageIDX = SKILL_TAB_TYPE( nSkillIDX );

	for (short nI=0; nI<MAX_LEARNED_SKILL_PER_PAGE; nI++) {
		if ( nSkillIDX == m_Skills.m_nPageIndex[ cPageIDX ][ nI ] )
			return ( cPageIDX*MAX_LEARNED_SKILL_PER_PAGE + nI );

		if ( nSlot < 0 && 0 == m_Skills.m_nPageIndex[ cPageIDX ][ nI ] ) {
			nSlot = cPageIDX*MAX_LEARNED_SKILL_PER_PAGE + nI;
		}
	}

	return nSlot;
}


//-------------------------------------------------------------------------------------------------
short CUserDATA::Skill_FindLearnedLevel( short nSkillIDX )
{
	short nFindSlot;

	nFindSlot = this->Skill_FindLearnedSlot( nSkillIDX );
	if ( nFindSlot >= 0 ) {
		return SKILL_LEVEL( this->m_Skills.m_nSkillINDEX[ nFindSlot ] );
	}

	return -1;
}

short CUserDATA::Skill_FindLearnedSlot ( short nSkillIDX )
{
	char cPageIDX = SKILL_TAB_TYPE( nSkillIDX );
	short nI, n1LevSkillIDX = SKILL_1LEV_INDEX( nSkillIDX );

	for (nI=0; nI<MAX_LEARNED_SKILL_PER_PAGE; nI++) {
		if ( 0 == m_Skills.m_nPageIndex[ cPageIDX ][ nI ] )
			continue;

		if ( n1LevSkillIDX == SKILL_1LEV_INDEX( m_Skills.m_nPageIndex[ cPageIDX ][ nI ] ) ) {
			return ( cPageIDX*MAX_LEARNED_SKILL_PER_PAGE + nI );
		}
	}

	return -1;
}


//-------------------------------------------------------------------------------------------------
bool CUserDATA::Skill_CheckJOB( short nSkillIDX )
{
	// 2004.3.18... icarus
	// 직업 체크...
	if ( !Check_JobCollection( SKILL_AVAILBLE_CLASS_SET(nSkillIDX) )	)
		return false;

	// 소속 조합 체크...
	if ( 0 == SKILL_AVAILBLE_UNION(nSkillIDX, 0 ) ) {
		// 첫번째 입력된 값이 없으면 통과...
		return true;
	}

	for (short nI=0; nI<SKILL_AVAILBLE_UNION_CNT; nI++) {
		if ( 0 == SKILL_AVAILBLE_UNION(nSkillIDX, nI ) ) {
			// 입력된 값중엔 없었다..
			return false;
		}
		
		if ( this->GetCur_JOHAP() == SKILL_AVAILBLE_UNION(nSkillIDX, nI) )
			return true;
	}
	return false;
}
bool CUserDATA::Skill_CheckLearnedSKILL( short nSkillIDX )
{
	if ( SKILL_NEED_SKILL_INDEX( nSkillIDX, 0 ) ) {
		short nValue;
		for (short nI=0; nI<SKILL_NEED_SKILL_CNT; nI++) {
			nValue = SKILL_NEED_SKILL_INDEX( nSkillIDX, nI );
			if ( 0 == nValue )
				return true;// goto _SKILL_IS_MATCH;

			nValue = this->Skill_FindLearnedLevel( nValue );
			if ( nValue < SKILL_NEDD_SKILL_LEVEL( nSkillIDX, nI ) )
				return false;	// RESULT_SKILL_LEARN_NEED_SKILL;
		}

//_SKILL_IS_MATCH:	;
	}
	return true;
}
bool CUserDATA::Skill_CheckNeedABILITY( short nSkillIDX )
{
	if ( SKILL_NEED_ABILITY_TYPE( nSkillIDX,0 ) ) {
		int iValue;
		for (short nI=0; nI<SKILL_NEED_ABILITY_TYPE_CNT; nI++) {
			if ( 0 == SKILL_NEED_ABILITY_TYPE( nSkillIDX,nI ) )
				break;

			iValue = Skill_GetAbilityValue( SKILL_NEED_ABILITY_TYPE( nSkillIDX,nI ) );
			if ( iValue < SKILL_NEED_ABILITY_VALUE(nSkillIDX,nI) )
				return false;//RESULT_SKILL_LEARN_NEED_ABILITY;
		}
	}

	return true;
}

// 스킬 습득 조건 ...
// 1. 직업 조건..
// 2. 보유한 스킬 목록 검색...
// 3. 조건 능력치 판단...
BYTE CUserDATA::Skill_LearnCondition( short nSkillIDX )
{
	if ( nSkillIDX < 1 || nSkillIDX >= g_SkillList.Get_SkillCNT() )
		return RESULT_SKILL_LEARN_INVALID_SKILL;

	// 0. 이미 보유한 스킬인지 판단..
	if ( Skill_FindLearnedSlot( nSkillIDX ) >= 0 )
		return RESULT_SKILL_LEARN_FAILED;

	// 1. 스킬 포인트 부족...
	if ( this->GetCur_SkillPOINT() < SKILL_NEED_LEVELUPPOINT(nSkillIDX) )
		return RESULT_SKILL_LEARN_OUTOFPOINT;

	// 2. 직업 조건 체크
	if ( !this->Skill_CheckJOB( nSkillIDX ) ) {
		return RESULT_SKILL_LEARN_NEED_JOB;
	}

	// 3. 습득한 스킬레벨 판단...
	if ( !this->Skill_CheckLearnedSKILL( nSkillIDX ) ) {
		return RESULT_SKILL_LEARN_NEED_SKILL;
	}

	// 4. 조건 능력치 판단...
	if ( !this->Skill_CheckNeedABILITY( nSkillIDX ) ) {
		return RESULT_SKILL_LEARN_NEED_ABILITY;
	}

	return RESULT_SKILL_LEARN_SUCCESS;
}


//-------------------------------------------------------------------------------------------------
// 스킬 레벨업 조건 ...
// 1. 필요 스킬 포인트..
// 2. 조건 능력치 판단.
BYTE CUserDATA::Skill_LevelUpCondition( short nCurLevelSkillIDX, short nNextLevelSkillIDX )
{
	if ( nNextLevelSkillIDX >= g_SkillList.Get_SkillCNT() ) {
		// 더이상 레벨업 할수 없다.
		return RESULT_SKILL_LEVELUP_FAILED;
	}

/*
	스킬 레벨업 제한 삭제...
	if ( SKILL_LEVEL( nCurLevelSkillIDX ) >= 10 ) {
		// 더이상 레벨업 할수 없다.
		return RESULT_SKILL_LEVELUP_FAILED;
	}
*/
	// 같은 종류의 스킬이고 배우려는 레벨이 현재 레벨의 다음 레벨인가 ??
	if ( SKILL_1LEV_INDEX( nCurLevelSkillIDX) != SKILL_1LEV_INDEX( nNextLevelSkillIDX ) ||
		 SKILL_LEVEL( nCurLevelSkillIDX )+1   != SKILL_LEVEL( nNextLevelSkillIDX ) ) {
		return RESULT_SKILL_LEVELUP_FAILED;
	}

	// 1. 스킬 포인트 부족...
	if ( this->GetCur_SkillPOINT() < SKILL_NEED_LEVELUPPOINT(nNextLevelSkillIDX) )
		return RESULT_SKILL_LEVELUP_OUTOFPOINT;

	// 2. 직업 조건 체크
	if ( !this->Skill_CheckJOB( nNextLevelSkillIDX ) ) {
		return RESULT_SKILL_LEVELUP_NEED_JOB;
	}

	// 3. 습득한 스킬레벨 판단...
	if ( !this->Skill_CheckLearnedSKILL( nNextLevelSkillIDX ) ) {
		return RESULT_SKILL_LEVELUP_NEED_SKILL;
	}

	// 4. 조건 능력치 판단...
	if ( !this->Skill_CheckNeedABILITY( nNextLevelSkillIDX ) ) {
		return RESULT_SKILL_LEVELUP_NEED_ABILITY;
	}

	return RESULT_SKILL_LEVELUP_SUCCESS;
}


#ifndef __SERVER
#include "Game.h"
#endif 
//-------------------------------------------------------------------------------------------------
// 스킬 사용시 조건 ...
// 1. 소모 수치..
// 2. 무기
bool CUserDATA::Skill_ActionCondition( short nSkillIDX )
{
	short nI;

	if ( SKILL_TYPE( nSkillIDX ) >= SKILL_TYPE_07 &&
		 SKILL_TYPE( nSkillIDX ) <= SKILL_TYPE_13 ) {
		if ( FLAG_ING_DUMB & this->GetCur_IngStatusFLAG() ) 
		{
#ifndef __SERVER
			AddMsgToChatWND( STR_CANT_CASTING_STATE, g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);
#endif
			// 벙어리 상태에서 사용할수 없는 스킬이다.
			return false;
		}
	} else
	if ( SKILL_TYPE_14 == SKILL_TYPE( nSkillIDX ) ) {
		if ( FLAG_ING_DUMB & this->GetCur_IngStatusFLAG() ) 
		{
#ifndef __SERVER
			AddMsgToChatWND( STR_CANT_CASTING_STATE, g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);
#endif
			// 벙어리 상태에서 사용할수 없는 스킬이다.
			return false;
		}

		short nNeedSummonCNT = NPC_NEED_SUMMON_CNT( SKILL_SUMMON_PET(nSkillIDX) ); 
		if ( this->GetCur_SummonCNT() + nNeedSummonCNT > this->GetMax_SummonCNT() ) {
			// 최대 소환 몹 가능 갯수 초과...
#ifndef __SERVER
			AddMsgToChatWND( STR_CANT_SUMMON_NPC, g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);
#endif
			return false;
		}
	}

	// 1. 마나 or .....
	int iCurValue;
	for (nI=0; nI<SKILL_USE_PROPERTY_CNT; nI++) 
	{
		if ( 0 == SKILL_USE_PROPERTY( nSkillIDX, nI ) )
			break;

		iCurValue = this->Skill_GetAbilityValue( SKILL_USE_PROPERTY( nSkillIDX, nI ) );
		if ( iCurValue < this->Skill_ToUseAbilityVALUE( nSkillIDX, nI ) )
		{
#ifndef __SERVER
			AddMsgToChatWND( STR_NOT_ENOUGH_PROPERTY, g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);
#endif
			return false;
		}
	}

	// 2.
	short nNeedWPN, nRWPN, nLWPN;
	
	if ( this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN ) {
		nRWPN = WEAPON_TYPE( this->GetCur_R_WEAPON() ); 
		nLWPN = SUBWPN_TYPE( this->GetCur_L_WEAPON() );
	} else {
		// 탑승시에는 필요 바디를 체크...
		nRWPN = PAT_ITEM_TYPE( this->GetCur_PET_BODY() );
		nLWPN = -1;
	}

	for (nI=0; nI<SKILL_NEED_WEAPON_CNT; nI++) 
	{
		nNeedWPN = SKILL_NEED_WEAPON( nSkillIDX, nI );
		
		if ( 0 == nNeedWPN )
		{
			/// 첫번째 필요장비가 0 이라면.. 필요장비가 필요없다.( 모든 칼럼이 0 이란 약속 )
			if( nI == 0 )
				return true;
			else /// 모든 필요무기는 앞쪽부터 채워진다.
				break;
		}

		if ( nRWPN == nNeedWPN || nLWPN == nNeedWPN ) {
			return true;
		}
	}

#ifndef __SERVER
	AddMsgToChatWND( STR_MISMATCH_CASTING_NEED_EQUIP, g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);
#endif

	return false;
}

//-------------------------------------------------------------------------------------------------
short CUserDATA::GetPassiveSkillAttackSpeed( float fCurSpeed, short nRightWeaponItemNo )
{
	t_AbilityINDEX eIndex;
	switch ( WEAPON_TYPE(nRightWeaponItemNo) ) {
		case 231 :	eIndex = AT_PSV_ATK_SPD_BOW;	break;	// 활
		case 233 :	// 투척기
		case 232 :	eIndex = AT_PSV_ATK_SPD_GUN;	break;	// 총
		case 251 :	// 카타르
		case 252 :	eIndex = AT_PSV_ATK_SPD_PAIR;	break;	// 이도류 

		default  :
			return 0;
	}

	return this->GetPassiveSkillValue( eIndex ) + (short)(fCurSpeed*this->GetPassiveSkillRate( eIndex )/100.f);
}
short CUserDATA::GetPassiveSkillAttackPower( int iCurAP, short nRightWeaponItemNo )
{
	t_AbilityINDEX eIndex;
	if ( 0 == nRightWeaponItemNo ) {
		// 맨손
		eIndex = AT_PSV_ATK_POW_NO_WEAPON;
	} else {
		switch ( WEAPON_TYPE(nRightWeaponItemNo) ) {
			// 한손검, 한손둔기, 
			case 211 : case 212 :				eIndex = AT_PSV_ATK_POW_1HAND_WPN;		break;
			// 양손검, 양손창, 양손도끼
			case 221 : case 222 : case 223 :	eIndex = AT_PSV_ATK_POW_2HAND_WPN;		break;
			// 활
			case 231 :							eIndex = AT_PSV_ATK_POW_BOW;			break;
			// 총, 투척기, 쌍총
			case 232 : case 233 : case 253 :	eIndex = AT_PSV_ATK_POW_GUN;			break;
			// 마법지팡이	// 마법완드
			case 241 : case 242 :				eIndex = AT_PSV_ATK_POW_STAFF_WAND;		break;
			// 카타르, 이도류
			case 251 : case 252 :				eIndex = AT_PSV_ATK_POW_KATAR_PAIR;		break;
			// 석궁
			case 271 :							eIndex = AT_PSV_ATK_POW_AUTO_BOW;		break;

			default :
				return 0;
		}
	}

	return ( this->GetPassiveSkillValue( eIndex ) + (short)( iCurAP * this->GetPassiveSkillRate( eIndex ) / 100.f ) );
}


//-------------------------------------------------------------------------------------------------
/// 스킬 습득 & 레벨업시..!!! :: 
/// return == true이면 CObjAVT->Update_SPEED() 호출 필요 !!!
BYTE CUserDATA::Skill_LEARN( short nSkillSLOT, short nSkillIDX, bool bSubPOINT )
{
	// btReturn & 0x02면 전체 갱신, & 0x01이면 속도 갱신
	BYTE btReturn = 0;

	// 스킬 포인트 소모 - 습득시에도 소모되도록 수정, 2004. 3. 16
	if ( bSubPOINT ) {
		this->SetCur_SkillPOINT( this->GetCur_SkillPOINT() - SKILL_NEED_LEVELUPPOINT(nSkillIDX) );
	}
	short nBeforeSkill = this->m_Skills.m_nSkillINDEX[ nSkillSLOT ];
	this->m_Skills.m_nSkillINDEX[ nSkillSLOT ] = nSkillIDX;		// 스킬 습득 !!!

	// 패시브 스킬이면 ???
	short nPassiveTYPE;
	if ( SKILL_TYPE_PASSIVE == SKILL_TYPE( nSkillIDX ) ) {
		for (short nI=0; nI<SKILL_INCREASE_ABILITY_CNT; nI++) {
			if ( 0 == SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI ) &&
				 0 == SKILL_CHANGE_ABILITY_RATE( nSkillIDX, nI ) )
				continue;

			// 수치로 올림 : 패시브 스킬은 비율로는 못올림 !!!
			if ( SKILL_INCREASE_ABILITY( nSkillIDX, nI ) >= AT_STR  &&
				 SKILL_INCREASE_ABILITY( nSkillIDX, nI ) <= AT_SENSE ) {
				nPassiveTYPE = SKILL_INCREASE_ABILITY( nSkillIDX, nI ) - AT_STR;

				if ( SKILL_CHANGE_ABILITY_RATE( nSkillIDX, nI ) ) {
					// 05.05.26 비율계산 추가...
					short nValue = SKILL_CHANGE_ABILITY_RATE( nSkillIDX, nI );
					if ( nBeforeSkill ) {
						nValue -= SKILL_CHANGE_ABILITY_RATE( nBeforeSkill, nI );
					}

					int iCurAbility = this->m_BasicAbility.m_nBasicA[ nPassiveTYPE ] + m_iAddValue[ SKILL_INCREASE_ABILITY( nSkillIDX, nI ) ];
					this->AddPassiveSkillRate( SKILL_INCREASE_ABILITY( nSkillIDX, nI ), nValue );
					m_PassiveAbilityFromRate[ nPassiveTYPE	] = (int)( iCurAbility * m_nPassiveRate[ SKILL_INCREASE_ABILITY( nSkillIDX, nI ) ] / 100.f );
				} else {
					short nValue = SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI );
					if ( nBeforeSkill ) {
						nValue -= SKILL_INCREASE_ABILITY_VALUE( nBeforeSkill, nI );
					}

					this->m_iAddValue[ SKILL_INCREASE_ABILITY( nSkillIDX, nI ) ] -= this->m_PassiveAbilityFromValue[ nPassiveTYPE ];
					this->m_PassiveAbilityFromValue[ nPassiveTYPE ] += nValue;
					this->m_iAddValue[ SKILL_INCREASE_ABILITY( nSkillIDX, nI ) ] += this->m_PassiveAbilityFromValue[ nPassiveTYPE ];
				}

				return 0x03;
			} else {
				nPassiveTYPE = SKILL_INCREASE_ABILITY( nSkillIDX, nI );
				if ( (nPassiveTYPE >= AT_PSV_ATK_POW_NO_WEAPON && nPassiveTYPE < AT_AFTER_PASSIVE_SKILL ) ||
					 (nPassiveTYPE >= AT_PSV_RES && nPassiveTYPE < AT_AFTER_PASSIVE_SKILL_2ND ) ) {
					// 패시브에의해 보정되는 값들...
					if ( SKILL_CHANGE_ABILITY_RATE( nSkillIDX, nI ) ) {
						// 비율 계산이면 ???
						short nValue = SKILL_CHANGE_ABILITY_RATE( nSkillIDX, nI );
						if ( nBeforeSkill ) {
							nValue -= SKILL_CHANGE_ABILITY_RATE( nBeforeSkill, nI );
						}
						this->AddPassiveSkillRate ( nPassiveTYPE, nValue );
					} else {
						short nValue = SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI );
						if ( nBeforeSkill ) {
							nValue -= SKILL_INCREASE_ABILITY_VALUE( nBeforeSkill, nI );
						}
						this->AddPassiveSkillValue( nPassiveTYPE, nValue );
					}

					switch( nPassiveTYPE ) {
						case AT_PSV_MAX_HP :		Cal_MaxHP ();			btReturn |= 0x01;	break;	// 주변에 통보 필요
						case AT_PSV_MAX_MP :		Cal_MaxMP ();								break;	// 주변에 통보 없이 재계산....
						case AT_PSV_DEF_POW :		Cal_DEFENCE ();								break;	// 주변에 통보 없이 재계산....
						case AT_PSV_WEIGHT :		// 무게 바뀜
							// this->m_btWeightRate = ;
							Cal_MaxWEIGHT ();
							btReturn |= 0x01;
							break;
						case AT_PSV_MOV_SPD :								btReturn |= 0x01;	break;	// 이속.. 주변에 통보 필요
						case AT_PSV_RECOVER_HP :	// 파티원들한테 변경 사항 통보 필요...
							Cal_RecoverHP ();
							btReturn |= 0x01;
							break;
						case AT_PSV_RECOVER_MP :	// 파티원들한테 변경 사항 통보 필요...
							Cal_RecoverMP ();
							btReturn |= 0x01;
							break;
						case AT_PSV_SAVE_MP :		// MP 절감 비율
							m_Battle.m_fRateUseMP	= ( 100 - this->GetCur_SaveMP() ) / 100.f;
							break;
						case AT_PSV_DROP_RATE :
							Cal_DropRATE ();
							break;

						case AT_PSV_RES :
							Cal_RESIST ();
							break;
						case AT_PSV_HIT	:
							Cal_HIT ();
							break;
						case AT_PSV_CRITICAL :
							Cal_CRITICAL ();
							break;
						case AT_PSV_AVOID :
							Cal_AvoidRATE ();
							break;
						case AT_PSV_SHIELD_DEF :
							Cal_DEFENCE ();
							break;
						case AT_PSV_IMMUNITY :
							;
							;
							break;

						default :
							if ( nPassiveTYPE <= AT_PSV_ATK_POW_KATAR_PAIR ) {
								// 공격력 변화...
								this->Cal_ATTACK ();
							} else 
							if ( nPassiveTYPE < AT_PSV_MOV_SPD ) {
								// 공속 변경...
								//tagITEM *pITEM = &m_Inventory.m_ItemEQUIP[ EQUIP_IDX_WEAPON_R ];
								//if ( pITEM->GetHEADER() ) {
								//	this->m_nPassiveAttackSpeed = this->GetPassiveSkillAttackSpeed( pITEM->GetItemNO() );
								//}
								btReturn |= 0x01;
							}
							break;
					}
					/// 여기서 2차적진 능력치 적용시켜 주지 않으면 안 맞다.
					this->Cal_BattleAbility();
				}
			}
		}
	}

	return btReturn;
}

//-------------------------------------------------------------------------------------------------
bool CUserDATA::Skill_DELETE( short nSkillSLOT, short nSkillIDX )
{
	if ( nSkillIDX == this->m_Skills.m_nSkillINDEX[ nSkillSLOT ] ) {
		this->m_Skills.m_nSkillINDEX[ nSkillSLOT ] = 0;			/// 스킬 삭제 !!!
		return true;
	}
	return false;
}


//-------------------------------------------------------------------------------------------------
void CUserDATA::InitPassiveSkill ()
{
	::ZeroMemory(&m_iAddValue   [ AT_PSV_ATK_POW_NO_WEAPON ],	sizeof(int)  *( AT_AFTER_PASSIVE_SKILL-AT_PSV_ATK_POW_NO_WEAPON ) );
	::ZeroMemory(&m_nPassiveRate[ AT_PSV_ATK_POW_NO_WEAPON ],	sizeof(short)*( AT_AFTER_PASSIVE_SKILL-AT_PSV_ATK_POW_NO_WEAPON ) );

	::ZeroMemory(&m_iAddValue   [ AT_PSV_RES ],	sizeof(int)  *( AT_AFTER_PASSIVE_SKILL_2ND-AT_PSV_RES ) );
	::ZeroMemory(&m_nPassiveRate[ AT_PSV_RES ],	sizeof(short)*( AT_AFTER_PASSIVE_SKILL_2ND-AT_PSV_RES ) );

	::ZeroMemory( m_PassiveAbilityFromRate,		sizeof(short) * BA_MAX );
	::ZeroMemory( m_PassiveAbilityFromValue,	sizeof(short) * BA_MAX );

	short nSlot, nSkillIDX, nI, nPassiveTYPE;
	for (nSlot=0; nSlot<MAX_LEARNED_SKILL_CNT; nSlot++) {
		if ( 0 == this->m_Skills.m_nSkillINDEX[ nSlot ] )
			continue;

		nSkillIDX = this->m_Skills.m_nSkillINDEX[ nSlot ];
		if ( SKILL_TYPE_PASSIVE == SKILL_TYPE( nSkillIDX ) ) {
			for (nI=0; nI<SKILL_INCREASE_ABILITY_CNT; nI++) {
				if ( 0 == SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI ) &&
					 0 == SKILL_CHANGE_ABILITY_RATE( nSkillIDX, nI ) )
					continue;
				
				// 수치로 올림 : 패시브 스킬은 비율로는 못올림 !!!
				if ( SKILL_INCREASE_ABILITY( nSkillIDX, nI ) >= AT_STR &&
					 SKILL_INCREASE_ABILITY( nSkillIDX, nI ) <= AT_SENSE ) {
					if ( SKILL_CHANGE_ABILITY_RATE( nSkillIDX, nI ) ) {
						// 05.05.26 비율계산 추가...
						this->AddPassiveSkillRate ( SKILL_INCREASE_ABILITY( nSkillIDX, nI ), SKILL_CHANGE_ABILITY_RATE( nSkillIDX, nI ) );
					} else {
						nPassiveTYPE = SKILL_INCREASE_ABILITY( nSkillIDX, nI ) - AT_STR;
						this->m_PassiveAbilityFromValue[ nPassiveTYPE ] += SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI );
					}
				} else {
					nPassiveTYPE = SKILL_INCREASE_ABILITY( nSkillIDX, nI );
					if ( (nPassiveTYPE >= AT_PSV_ATK_POW_NO_WEAPON && nPassiveTYPE <= AT_AFTER_PASSIVE_SKILL ) ||
						 (nPassiveTYPE >= AT_PSV_RES && nPassiveTYPE < AT_AFTER_PASSIVE_SKILL_2ND ) ) {
						if ( SKILL_CHANGE_ABILITY_RATE( nSkillIDX, nI ) ) {
							// 05.05.26 비율계산 이면 ???
							this->AddPassiveSkillRate ( nPassiveTYPE, SKILL_CHANGE_ABILITY_RATE( nSkillIDX, nI ) );
						} else {
							this->AddPassiveSkillValue( nPassiveTYPE, SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI ) );
						}
					}
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
/// 2004 / 2 /10 :nAvy수정( return 0에서 해당 SkillIdx로 STB에서 필요 포인트를 가져온다)
/// 2004 / 6 /10 :nAvy수정( STB 구조변경 - 다음레벨업을 위한 포인트는 다음레벨에서 가져온다 )
short CUserDATA::Get_NeedPoint2SkillUP (short nSkillSLOT) {
	_ASSERT( nSkillSLOT >= 0 && nSkillSLOT <= MAX_LEARNED_SKILL_CNT );
	short nSkillIDX = m_Skills.m_nSkillINDEX[ nSkillSLOT ];

	short nNextLevelSkillIDX = nSkillIDX + 1;

	if ( nNextLevelSkillIDX >= g_SkillList.Get_SkillCNT() ) {
		// 더이상 레벨업 할수 없다.
		return 0;
	}

	// 같은 종류의 스킬이고 배우려는 레벨이 현재 레벨의 다음 레벨인가 ??
	if ( SKILL_1LEV_INDEX( nSkillIDX) != SKILL_1LEV_INDEX( nNextLevelSkillIDX ) ||
		 SKILL_LEVEL( nSkillIDX )+1   != SKILL_LEVEL( nNextLevelSkillIDX ) ) {
		return 0;
	}


	//TODO:: 여기서 skill stb의 컬럼에 있는 값을 전송...
	return SKILL_NEED_LEVELUPPOINT( nNextLevelSkillIDX );
}

//-------------------------------------------------------------------------------------------------
BYTE CUserDATA::Quest_GetRegistered( int iQuestIDX )
{
	if ( iQuestIDX ) {
		for (BYTE btI=0; btI<QUEST_PER_PLAYER; btI++) {
			if ( iQuestIDX == m_Quests.m_QUEST[ btI ].GetID() )
				return btI;
		}
	}

	return QUEST_PER_PLAYER;
}

//-------------------------------------------------------------------------------------------------
short CUserDATA::Quest_Append ( int iQuestIDX )
{
	if ( iQuestIDX ) {
		short nI;
		for (nI=0; nI<QUEST_PER_PLAYER; nI++) {
			if ( iQuestIDX == m_Quests.m_QUEST[ nI ].GetID() ) {
				m_Quests.m_QUEST[ nI ].Init ();
				m_Quests.m_QUEST[ nI ].SetID( iQuestIDX, true );
				return nI;
			}
		}
		for (nI=0; nI<QUEST_PER_PLAYER; nI++) {
			if ( 0 == m_Quests.m_QUEST[ nI ].GetID() ) {
				m_Quests.m_QUEST[ nI ].Init ();
				m_Quests.m_QUEST[ nI ].SetID( iQuestIDX, true );
				return nI;
			}
		}
	}

	return -1;
}
bool CUserDATA::Quest_Append ( BYTE btSlot, int iQuestIDX )
{
	if ( btSlot >= QUEST_PER_PLAYER )
		return false;

	if ( m_Quests.m_QUEST[ btSlot ].GetID() )
		return false;

	m_Quests.m_QUEST[ btSlot ].Init ();
	m_Quests.m_QUEST[ btSlot ].SetID( iQuestIDX, true );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CUserDATA::Quest_Delete( int iQuestIDX )
{
	for (short nI=0; nI<QUEST_PER_PLAYER; nI++) {
		if ( iQuestIDX == m_Quests.m_QUEST[ nI ].GetID() ) {
			m_Quests.m_QUEST[ nI ].Init ();
			return true;
		}
	}
	return false;
}
bool CUserDATA::Quest_Delete ( BYTE btSLOT, int iQuestID )
{
	if ( btSLOT >= QUEST_PER_PLAYER )
		return false;

	if ( iQuestID != m_Quests.m_QUEST[ btSLOT ].GetID() )
		return false;

	m_Quests.m_QUEST[ btSLOT ].Init ();
	return true;
}

bool	CUserDATA::Quest_SubITEM( tagITEM &sSubITEM )
{
	short nFindSLOT = m_Inventory.FindITEM( sSubITEM );
	if ( nFindSLOT >= 0 ) 
	{
		this->Sub_ITEM( nFindSLOT, sSubITEM );
#ifndef __SERVER
		g_itMGR.AppendChatMsg( sSubITEM.SubtractQuestMESSAGE(), g_dwWHITE );
#endif
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
#ifndef	__SERVER
#include "Game_FUNC.h"
#endif
bool CUserDATA::Reward_InitSKILL (void)
{
	short nSkillIDX, n1LevSkillIDX;
	int iRecoverySP = 0;

	for (int iS=MAX_LEARNED_SKILL_PER_PAGE; iS<MAX_LEARNED_SKILL_CNT; iS++) {
		nSkillIDX = this->m_Skills.m_nSkillINDEX[ iS ];
		if ( 0 == nSkillIDX )
			continue;

		this->m_Skills.m_nSkillINDEX[ iS ] = 0;

		n1LevSkillIDX = SKILL_1LEV_INDEX( nSkillIDX );
		do {
			iRecoverySP += SKILL_NEED_LEVELUPPOINT( nSkillIDX );
			nSkillIDX --;
		} while( nSkillIDX > 0 && n1LevSkillIDX == SKILL_1LEV_INDEX( nSkillIDX ) );
	}
	this->AddCur_SkillPOINT( iRecoverySP );

	// 0페이지의 기본 스킬을 걍 둔다.
	::ZeroMemory( &m_Skills.m_nSkillINDEX[ MAX_LEARNED_SKILL_PER_PAGE ], sizeof(short) * ( MAX_LEARNED_SKILL_CNT-MAX_LEARNED_SKILL_PER_PAGE ) );

	// 초기화 됐으니 상태를 보내자...
	this->InitPassiveSkill ();
	this->UpdateCur_Ability ();
	this->Quest_CHANGE_SPEED ();

	return true;
}

bool CUserDATA::Reward_InitSTATUS (void)
{
	// 케릭터의 스텟을 초기화 한다. 
	// 보상되는 포인트식 수정... 2004. 6. 7 by icarus
	int iBPoint = (int)( ( this->GetCur_LEVEL()-1 ) * ( this->GetCur_LEVEL()+24 ) * 0.4f + ( this->GetCur_LEVEL()-1 )*0.8f + 0.5f );
	this->SetCur_BonusPOINT( iBPoint );
	
	short nRace = this->GetCur_RACE();

	this->SetDef_STR   ( AVATAR_STR  ( nRace ) );
	this->SetDef_DEX   ( AVATAR_DEX  ( nRace ) );
	this->SetDef_INT   ( AVATAR_INT  ( nRace ) );
	this->SetDef_CON   ( AVATAR_CON  ( nRace ) );
	this->SetDef_CHARM ( AVATAR_CHARM( nRace ) );
	this->SetDef_SENSE ( AVATAR_SENSE( nRace ) );

	// 초기화 됐으니 상태를 보내자...
	this->InitPassiveSkill ();
	this->UpdateCur_Ability ();
	this->Quest_CHANGE_SPEED ();

#ifndef	__SERVER
	int iEffect = GF_GetEffectUseFile( "3DData\\Effect\\healing_01.eft" );
	GF_EffectOnObject( g_pAVATAR->Get_INDEX(), iEffect );
#endif

	return true;
}

bool CUserDATA::Reward_ITEM( tagITEM &sITEM, BYTE btRewardToParty, BYTE btQuestSLOT )
{
	if ( btRewardToParty ) {
		;
		;
		;
	} else {
		if ( ITEM_TYPE_QUEST == sITEM.GetTYPE() ) {
			// 퀘스트 인벤토리에 넣기...
			if ( btQuestSLOT >= QUEST_PER_PLAYER )
				return false;

			this->m_Quests.m_QUEST[ btQuestSLOT ].AddITEM( sITEM );
		} else {
			if ( !sITEM.IsEnableDupCNT() && 0 == sITEM.GetOption() ) {
				// 장비이고 옵션이 없으면 : 소켓 결정.
				switch( ITEM_RARE_TYPE( sITEM.GetTYPE(), sITEM.GetItemNO() ) ) {
					case 1 :	// 무조건
						sITEM.m_bHasSocket = 1;
						sITEM.m_bIsAppraisal = 1;
						break;
					case 2 :	// 계산
						if ( ITEM_QUALITY( sITEM.GetTYPE(), sITEM.GetItemNO() ) + 60 - RANDOM(400) > 0 ) {
							sITEM.m_bHasSocket = 1;
							sITEM.m_bIsAppraisal = 1;
							break;
						}
				}
			}

			this->Add_ItemNSend( sITEM );
		}
#ifndef __SERVER
		g_itMGR.AppendChatMsg( sITEM.GettingQuestMESSAGE(), g_dwWHITE );
#endif

	}

	return true;
}

bool CUserDATA::Reward_ABILITY( bool bSetOrAdj, int iType, int iValue, BYTE btRewardToParty )
{
	if ( btRewardToParty ) {
		;
		;
		;
	} else {
		if ( bSetOrAdj )
			this->Set_AbilityValueNSend( iType, iValue );
		else
			this->Add_AbilityValueNSend( iType, iValue );
	}

	return true;
}

bool CUserDATA::Reward_CalEXP( BYTE btEquation, int iBaseValue, BYTE btRewardToParty )
{
	int iR;

	if ( btRewardToParty ) {
		;
		;
		;
	} else {
		iR = CCal::Get_RewardVALUE( btEquation, iBaseValue, this, 0 );
		this->Add_ExpNSend( iR );
	}

	return true;
}

bool CUserDATA::Reward_CalMONEY( BYTE btEquation, int iBaseValue, BYTE btRewardToParty, short nDupCNT )
{
	int iR;

	if ( btRewardToParty ) {
	} else {
		iR = CCal::Get_RewardVALUE( btEquation, iBaseValue, this, nDupCNT );
		this->Add_MoneyNSend( iR, GSV_REWARD_MONEY );
	}
	return true;
}

bool CUserDATA::Reward_CalITEM( BYTE btEquation, int iBaseValue, BYTE btRewardToParty, int iItemSN, short nItemOP, BYTE btQuestSLOT )
{
	tagITEM sITEM;

	sITEM.Init( iItemSN );

	int iR;
	if ( btRewardToParty ) {
	} else {
		if ( sITEM.IsEnableDupCNT() ) {
			iR = CCal::Get_RewardVALUE( btEquation, iBaseValue, this, 0 );
			if ( iR > 0 ) {
				sITEM.m_uiQuantity = iR;		// 수량...
				if ( ITEM_TYPE_QUEST == sITEM.GetTYPE() ) {
					if ( btQuestSLOT >= QUEST_PER_PLAYER )
						return false;

					this->m_Quests.m_QUEST[ btQuestSLOT ].AddITEM( sITEM );
				} else {
					this->Add_ItemNSend( sITEM );
				}
			}
		} else {
			// 장비
			if ( nItemOP && nItemOP < 300 ) {
				sITEM.m_bIsAppraisal = 1;
				sITEM.m_bHasSocket   = 0;
				sITEM.m_nGEM_OP		 = nItemOP;
			} else {
				// 장비고 옵션이 없는 경우다...소켓 결정.
				switch( ITEM_RARE_TYPE( sITEM.GetTYPE(), sITEM.GetItemNO() ) ) {
					case 1 :	// 무조건
						sITEM.m_bHasSocket = 1;
						sITEM.m_bIsAppraisal = 1;
						break;
					case 2 :	// 계산
						if ( ITEM_QUALITY( sITEM.GetTYPE(), sITEM.GetItemNO() ) + 60 - RANDOM(400) > 0 ) {
							sITEM.m_bHasSocket = 1;
							sITEM.m_bIsAppraisal = 1;
							break;
						}
				}
			}
			this->Add_ItemNSend( sITEM );
		}
	}

	return true;
}

void CUserDATA::Set_ITEM(short nListRealNO, tagITEM& sITEM)
{
	tagITEM oldItem = Get_InventoryITEM (nListRealNO);
	if( !oldItem.IsEmpty() )
		ClearITEM( nListRealNO );

	if( !sITEM.IsEmpty() )
		Add_ITEM( nListRealNO, sITEM );
}

short	CUserDATA::Add_ITEM(short nListRealNO, tagITEM &sITEM)
{	
	return  m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);		
}

void	CUserDATA::Sub_ITEM(short nListRealNO, tagITEM &sITEM)
{	
	m_Inventory.SubtractITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);			
}

short	CUserDATA::Add_CatchITEM (short nListRealNO, tagITEM &sITEM)
{	
	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT );
}
//-------------------------------------------------------------------------------------------------
void  CUserDATA::SetCur_HP (short nValue)
{	

#ifndef __SERVER
	assert( 0 != nValue );
#endif

	this->m_GrowAbility.m_nHP=nValue;				
}	// 생명력

//-------------------------------------------------------------------------------------------------
/// 2차전직 때문에 상승되는 능력치 적용
void CUserDATA::Apply_2ndJob_Ability ( void )
{
#ifdef	__APPLY_2ND_JOB
	this->m_Battle.m_nImmunity = 0;
	switch( GetCur_JOB() ) {
		case CLASS_SOLDIER_121	:	// 2차 나이트
		case CLASS_SOLDIER_122	:	// 2차 챔프
		case CLASS_MAGICIAN_221	:	// 2차 메지션
		case CLASS_MAGICIAN_222	:	// 2차 클러릭
		case CLASS_MIXER_321	:	// 2차 레이더
		case CLASS_MIXER_322	:	// 2차 스카우트
		case CLASS_MERCHANT_421	:	// 2차 부즈주아
		case CLASS_MERCHANT_422	:	// 2차 아티쟌
			this->m_Battle.m_nMaxHP += 300;
			this->m_Battle.m_nATT   += 30;
			this->m_Battle.m_nDEF   += 25;
			this->m_Battle.m_nRES   += 20;
			// this->m_Battle.m_nImmunity = 30;
			break;
	}
#endif
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
