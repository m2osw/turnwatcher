//===============================================================================
// Copyright (c) 2005-2013 by Made to Order Software Corporation
// 
// All Rights Reserved.
// 
// The source code in this file ("Source Code") is provided by Made to Order Software Corporation
// to you under the terms of the GNU General Public License, version 2.0
// ("GPL").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution.
// 
// By copying, modifying or distributing this software, you acknowledge
// that you have read and understood your obligations described above,
// and agree to abide by those obligations.
// 
// ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED "AS IS." THE AUTHOR MAKES NO
// WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
// COMPLETENESS OR PERFORMANCE.
//===============================================================================




// MOLIB
//
#include "mo/mo_random.h"
#include "mo/mo_array.h"
#include "mo/mo_props.h"

// LOCAL
//
#include "character.h"
#include "SingletonManager.h"

namespace Combatant
{

namespace
{
	molib::moRandom		g_random;

	char* my_itoa( const int val )
	{
		static char buf[64];
		sprintf( buf, "%d", val );
		return buf;
	}

	int roll_dice( int faces = 20 )
	{
		// faces should not be larger than 100, though we can accept 255
		// here we compute the limit for the number of faces
		// for 20 faces, the limit is 240; for 100 faces, it is 200; etc.
		// (240 was the old hard coded limit)
		if(faces < 2) {
			faces = 2;
		}
		else if(faces > 100) {
			faces = 100;
		}
		int limit = (255 / faces) * faces;
		int val;
		int cnt = 10;
		do {
			cnt--;
			if(cnt == 0) {
				// Should never happen...
				// With 1 to 3 attempts we should have an acceptable result!
				return faces;
			}
			val = g_random.Random() >> 24;
		} while(val >= limit && cnt > 0);

		return val % faces + 1;
	}

	const molib::moWCString NAME_STATS  ("STATS");
	const molib::moWCString NAME_VALUE  ("VALUE");

	// moNames for each attribute to save
	//
	const molib::moWCString	g_name 		("NAME");
	const molib::moWCString	g_publicName 	("PUBLIC_NAME");
	const molib::moWCString	g_notes		("NOTES");
	const molib::moWCString	g_monster	("MONSTER");
	const molib::moWCString	g_hitDice	("HITDICE");
	const molib::moWCString	g_baseHP   	("BASEHP");
	const molib::moWCString	g_tempHP   	("TEMPHP");
	const molib::moWCString	g_damage   	("DAMAGE");
	const molib::moWCString	g_stabilized	("STABILIZED");
	const molib::moWCString	g_justdropped	("JUSTDROPPED");
	const molib::moWCString	g_status   	("STATUS");
	const molib::moWCString	g_effectsBag	("EFFECTS");
	const molib::moWCString	g_position 	("POSITION");
	const molib::moWCString	g_subPosition	("SUBPOSITION");
	const molib::moWCString	g_manualPos	("MANUALPOS");
}


Character::Character() :
	f_monster       (false),
	f_hitDice       (0),
	f_baseHP        (0),
	f_tempHP        (0),
	f_damage        (0),
	f_stabilized    (false),
	f_justdropped   (false),
	f_status        (Normal),
	f_position      (0),
	f_subPosition   (0),
	f_manualPos     (0),
	f_forcePosition (-1),
	f_deleted       (false)
{
	auto statMgr = Application::Manager::Instance().lock()->GetStatMgr().lock();
	assert( statMgr );

	OnStatsChanged();

	// Listen for changes in supported stats
	//
	f_statConnection = statMgr
					 ->signal_changed().connect(
					 sigc::mem_fun( *this, &Character::OnStatsChanged )
					 );
}


Character::~Character()
{
	f_statConnection.disconnect();
}
	
	
#if 0
void Character::name( const molib::moWCString& val )
{ 
	f_name = val;
#ifdef DEBUG
	std::cout << "Character name changed: " << f_name.c_str() << std::endl;
#endif
	f_signalChanged.emit();	
}

void Character::public_name( const molib::moWCString& val )
{
	f_publicName = val;
#ifdef DEBUG
	std::cout << "Character public name changed: " << f_publicName.c_str() << std::endl;
#endif
	f_signalChanged.emit();
}
#endif

void Character::OnStatsChanged()
{
	AddStat();
}


void Character::AddStat()
{
	// Set up the stats first time
	//
	const auto&	statMap = Application::Manager::Instance().lock()->GetStatMgr().lock()->GetStats();
	for( auto& statPair : statMap )
	{
		auto stat = statPair.second;
		if( !stat )
		{
			continue;
		}

		const molib::mo_name_t	id	= stat->id();
		Attribute::Value::Pointer	value	= GetStat( id );
		//
		if( value )
		{
			// Just replace the stat if it already exists
			//
			value->stat( stat );
		}
		else
		{
			// Otherwise, create it
			//
			f_values[id] = CreateStat( stat );
		}
	}
}


Attribute::Value::Pointer	Character::CreateStat( Attribute::Stat::Pointer stat )
{
	Attribute::Value::Pointer	value;
	value.reset( new Attribute::Value );
	assert(value);
	value->stat( stat );
	return value;
}


#ifdef WANT_EFFECTS
void Character::LoadEffects( molib::moPropBagRef _effectsBag )
{
	int count = _effectsBag.Count();
	
	for( int idx = 0; idx < count; ++idx )
	{
		if( _effectsBag[idx].HasProp() )
		{
			Effects::Effect::Pointer effect;
			effect.reset( new Effects::Effect );
			effect->Load( _effectsBag[idx] );
			addEffect( effect );
		}
	}
}


void Character::SaveEffects( molib::moPropBagRef _effectsBag )
{
	EffectEntries::iterator	iter= f_effects.begin();
	EffectEntries::iterator	end = f_effects.end();

	for( ; iter != end; ++iter )
	{
		EffectEntry& entry = *iter;
		const int BUFLEN = 16;
		char buf[BUFLEN+1];
		snprintf( buf, BUFLEN, "%d", entry.f_id );
		molib::moPropBagRef _propBag( buf );
		_propBag.NewProp();
		entry.f_effect->Save( _propBag );
		_effectsBag.Set( buf, _propBag );
	}
}
#endif


void Character::CopyStats( Character::Pointer ch )
{
	assert(ch);
	//
	f_values.clear();

	for( auto& valueMap : ch->f_values )
	{
		Attribute::Value::Pointer	source	= valueMap.second;
		assert(source);
		Attribute::Value::Pointer	target;
		target.reset( new Attribute::Value );
		assert(target);
		//
		target->Copy( source );
		//
		Attribute::Stat::Pointer stat( source->stat() );
		assert(stat);
		//
		const molib::mo_name_t	id = stat->id();
		f_values[id] = target;
	}
}


#if !defined(DEMO_VERSION)
void Character::LoadStats( molib::moPropBagRef& propBag )
{
	//const molib::moNamePool& pool = molib::moNamePool::GetNamePool();

	molib::moPropArrayRef	array( NAME_STATS );
	array = propBag.Get( NAME_STATS );

	if( !array.HasProp() ) return;
	//
	const unsigned long count = array.CountIndexes();

	for( unsigned long idx = 0; idx < count; ++idx )
	{
		const int			item_no		= array.ItemNoAtIndex( idx );
		molib::moPropSPtr	prop_ptr	= array.Get( item_no );
		molib::moName		prop_name	= prop_ptr->GetName();
		molib::moPropBagRef	prop(prop_name);
		prop.NewProp();
		prop.GetProperty()->Copy( *prop_ptr );

		try
		{
			Attribute::Value::Pointer	value;
			value.reset( new Attribute::Value );
			assert(value);
			//
			value->Load( prop );
			//
			const molib::mo_name_t	id = value->stat()->id();
			f_values[id] = value;
		}
		catch( const molib::moError& x )
		{
			// Forget this stat--we cannot find it!
			//
			std::cerr << "Character::LoadStats(): Caught moError: " << x.Message() << std::endl;
		}
	}
}


void Character::SaveStats( molib::moPropBagRef& propBag )
{
	molib::moPropArrayRef	array( NAME_STATS );
	array.NewProp();
	//
	ValueMap::iterator			iter = f_values.begin();
	const ValueMap::iterator	end  = f_values.end();

	for( int id = 0; iter != end; ++iter, ++id )
	{
		Attribute::Value::Pointer	value = iter->second;

		// Don't save if deleted
		//
		Attribute::Stat::Pointer stat = value->stat();
		if( stat->deleted() )
			continue;

		molib::moPropBagRef prop_bag( NAME_VALUE );
		prop_bag.NewProp();
		value->Save( prop_bag );
		array.Set( id, prop_bag.GetProperty() );
	}

	propBag += array;
}

#endif // !defined(DEMO_VERSION)


void Character::Load( molib::moPropBagRef& propBag )
{
#if defined(DEMO_VERSION)
      // No saving allowed for demo version!
      return;
#else
	molib::moPropStringRef	_name		(g_name		); // name of character
	molib::moPropStringRef	_publicName	(g_publicName	); // public name of character (on HUD)
	molib::moPropStringRef	_notes		(g_notes  	); // Misc notes
	molib::moPropIntRef	_monster	(g_monster	); // is this character a monster or pc?

	molib::moPropIntRef	_hitDice	(g_hitDice	); // Hitdice for the character
	molib::moPropIntRef	_baseHP		(g_baseHP	); // Base hitpoints the character has
	molib::moPropIntRef	_tempHP		(g_tempHP	); 
	molib::moPropIntRef	_damage		(g_damage	); // Current damage
	molib::moPropIntRef	_stabilized	(g_stabilized	); // If dying, character may be stabilized
	molib::moPropIntRef	_justdropped	(g_justdropped	); // If true, character just dropped below 0 hitpoints

	molib::moPropIntRef	_status		(g_status	); // normal, delayed or readied action
	molib::moPropIntRef	_position	(g_position	); // the real initiative position (1 based, if 0, not sorted)
	molib::moPropIntRef	_subPosition	(g_subPosition	); // tie-breaker for similar initiatives.
	molib::moPropIntRef	_manualPos	(g_manualPos	); // if the character is moved manually in initiative order (delayed, readied or move)

	molib::moPropBagRef	_effectsBag	(g_effectsBag	); // Bag containing running effects

	_name			.Link( propBag ); if( _name			.HasProp() ) f_name			= _name;
	_notes			.Link( propBag ); if( _notes		.HasProp() ) f_notes		= _notes;
	_monster		.Link( propBag ); if( _monster		.HasProp() ) f_monster		= _monster? true: false;
	_hitDice		.Link( propBag ); if( _hitDice		.HasProp() ) f_hitDice		= _hitDice;
	_baseHP			.Link( propBag ); if( _baseHP		.HasProp() ) f_baseHP		= _baseHP;
	_tempHP			.Link( propBag ); if( _tempHP		.HasProp() ) f_tempHP		= _tempHP;
	_damage			.Link( propBag ); if( _damage		.HasProp() ) f_damage		= _damage;
	_stabilized		.Link( propBag ); if( _stabilized	.HasProp() ) f_stabilized	= _stabilized? true: false;
	_justdropped	.Link( propBag ); if( _justdropped	.HasProp() ) f_justdropped	= _justdropped? true: false;
	_status			.Link( propBag ); if( _status		.HasProp() ) f_status		= (Status) (int) _status;
	_position		.Link( propBag ); if( _position		.HasProp() ) f_position		= _position;
	_subPosition	.Link( propBag ); if( _subPosition	.HasProp() ) f_subPosition	= _subPosition;
	_manualPos		.Link( propBag ); if( _manualPos	.HasProp() ) f_manualPos	= _manualPos;
	//
	_publicName.Link( propBag );
	if( _publicName.HasProp() )
	{
		f_publicName = _publicName;
	}
	else if( _name.HasProp() )
	{
		if( f_monster )
		{
			f_publicName = "Monster";
		}
		else
		{
			f_publicName = _name;
		}
	}
	//
#ifdef WANT_EFFECTS
	_effectsBag	.Link( propBag );

	if( _effectsBag.HasProp() )
	{
		LoadEffects( _effectsBag );
	}
#endif

	LoadStats( propBag );

#endif // !DEMO_VERSION
}


void Character::Save( molib::moPropBagRef& propBag )
{
#if defined(DEMO_VERSION)
	// No saving allowed for demo version!
	return;
#else
	if( f_deleted )
	{
		// Don't bother saving if deleted flag set...
		//
		return;
	}

	molib::moPropStringRef	_name		(g_name		); // name of character
	molib::moPropStringRef	_publicName	(g_publicName	); // name of character on HUD
	molib::moPropStringRef	_notes		(g_notes  	); // Misc notes
	molib::moPropIntRef	_monster	(g_monster	); // is this character a monster or pc?

	molib::moPropIntRef	_hitDice	(g_hitDice	); // Hitdice for the character
	molib::moPropIntRef	_baseHP		(g_baseHP	); // Base hitpoints the character has
	molib::moPropIntRef	_tempHP		(g_tempHP	); 
	molib::moPropIntRef	_damage		(g_damage	); // Current damage
	molib::moPropIntRef	_stabilized	(g_stabilized	); // If dying, character may be stabilized
	molib::moPropIntRef	_justdropped	(g_justdropped	); // If true, character just dropped below 0 hitpoints

	molib::moPropIntRef	_status		(g_status	); // normal, delayed or readied action
	molib::moPropIntRef	_position	(g_position	); // the real initiative position (1 based, if 0, not sorted)
	molib::moPropIntRef	_subPosition	(g_subPosition	); // tie-breaker for similar initiatives.
	molib::moPropIntRef	_manualPos	(g_manualPos	); // if the character is moved manually in initiative order (delayed, readied or move)

	molib::moPropBagRef	_effectsBag	(g_effectsBag	); // Bag containing running effects

	// Save properties in bag for later retrieval
	//
	_name		.NewProp();	_name		= f_name		;
	_publicName	.NewProp();	_publicName	= f_publicName		;
	_notes		.NewProp();	_notes		= f_notes		;
	_monster	.NewProp();	_monster	= f_monster? 1: 0	;
	_hitDice	.NewProp();	_hitDice	= f_hitDice		;
	_baseHP		.NewProp();	_baseHP		= f_baseHP		;
	_tempHP		.NewProp();	_tempHP		= f_tempHP		;
	_damage		.NewProp();	_damage		= f_damage		;
	_stabilized	.NewProp();	_stabilized	= f_stabilized? 1: 0	;
	_justdropped	.NewProp();	_justdropped	= f_justdropped? 1: 0	;
	_status		.NewProp();	_status		= (int) f_status	;	
	_position	.NewProp();	_position	= f_position		;
	_subPosition	.NewProp();	_subPosition	= f_subPosition		;
	_manualPos	.NewProp();	_manualPos	= f_manualPos		;
	//
	propBag += _name;
	propBag += _publicName;
	propBag += _notes;
	propBag += _monster;
	propBag += _hitDice;
	propBag += _baseHP;
	propBag += _tempHP;
	propBag += _damage;
	propBag += _stabilized;
	propBag += _justdropped;
	propBag += _status;
	propBag += _position;
	propBag += _subPosition;
	propBag += _manualPos;

	SaveStats( propBag );

#ifdef WANT_EFFECTS
	_effectsBag.NewProp();
	SaveEffects( _effectsBag );
	propBag.Set( molib::moIndexOrName(_effectsBag.GetName()), _effectsBag );
#endif

#endif // !DEMO_VERSION
}


void Character::Duplicate( Character::Pointer ch )
{
	Copy( ch );
	f_status      = Normal;
	f_stabilized  = 0;
	f_justdropped = 0;
	f_damage      = 0;
	f_position    = 0;
	f_subPosition = 0;
	f_manualPos   = 0;
}


void Character::Copy( Character::Pointer ch )
{
	f_name        = ch->f_name;
	f_publicName  = ch->f_publicName;
	f_notes       = ch->f_notes;
	f_monster     = ch->f_monster;
	f_hitDice     = ch->f_hitDice;
	f_baseHP      = ch->f_baseHP;
	f_tempHP      = ch->f_tempHP;
	f_damage      = ch->f_damage;
	f_stabilized  = ch->f_stabilized;
	f_justdropped = ch->f_justdropped;
	f_status      = ch->f_status;
	f_position    = ch->f_position;
	f_subPosition = ch->f_subPosition;
	f_manualPos   = ch->f_manualPos;

	CopyStats( ch );
#ifdef WANT_EFFECTS
	Effects::Effect::List effects;
	ch->getEffects( effects );
	setEffects( effects );
#endif
}


void Character::Copy( LegacyCharacter::Pointer lch )
{
	f_name        = lch->name()        ;
	f_monster     = lch->monster()     ;
	f_baseHP      = lch->maxHP()       ;
	f_damage      = lch->damage()      ;
	f_stabilized  = lch->stabilized()  ;
	f_justdropped = lch->justdropped() ;
	f_position    = lch->position()    ;
	f_subPosition = 0;
	f_manualPos   = 0;

	switch( lch->status() )
	{
		case LegacyCharacter::InternalNormal  : f_status = Normal ;	break;
		case LegacyCharacter::InternalDelayed : f_status = Delayed;	break;
		case LegacyCharacter::InternalReadied : f_status = Readied;	break;
		default              				  : f_status = Normal;
	}

	auto statMgr = Application::Manager::Instance().lock()->GetStatMgr().lock();
	const molib::mo_name_t initId   = statMgr->initId();
	const molib::mo_name_t spotId   = statMgr->spotId();
	const molib::mo_name_t listenId = statMgr->listenId();
	const molib::mo_name_t willId   = statMgr->willId();
	//
	setMod( initId  , lch->init()   );
	setMod( spotId  , lch->spot()   );
	setMod( listenId, lch->listen() );
	setMod( willId  , lch->will()   );
	//
	setRoll( initId  , lch->initRoll()   );
	setRoll( spotId  , lch->spotRoll()   );
	setRoll( listenId, lch->listenRoll() );
	setRoll( willId  , lch->willRoll()   );
}


int Character::makeRoll( const molib::mo_name_t id )
{
	Attribute::Value::Pointer	value( GetStat( id ) );
	assert( value );

	int roll  = 0;
	int dice  = 1;
	int faces = 20;
	
	try
	{
		Attribute::Stat::Pointer stat = value->stat();
		assert(stat);
		//
		roll  = stat->modifier();	// this is the roll modified; the other one is the character modifier
		dice  = stat->dice();
		faces = stat->faces();
	}
	catch( const std::exception& )
	{
	}

	if(dice <= 0) 
	{
		dice = 1;
	}

	while(dice > 0)
	{
		roll += roll_dice( faces );
		--dice;
	}

	int mod  = 0;

	try
	{
		mod = getMod( value, false /*with_ability*/ );
	}
	catch( const std::exception& )
	{
		// TODO: Why do we ignore this exception?!
	}
		
	return roll + mod;
}


void Character::makeAllRolls()
{
	ValueMap::const_iterator iter = f_values.begin();
	ValueMap::const_iterator end  = f_values.end();

	for( ; iter != end; ++iter )
	{
		Attribute::Value::Pointer	value = iter->second;
		assert(value);
		//
		Attribute::Stat::Pointer	stat = value->stat();
		assert(stat);
		//
		value->roll( makeRoll( stat->id() ) );
	}
}


int Character::getPosition() const
{
	// should we also check the status here?
	return f_position;
}


void Character::setPosition( const int position )
{
	f_position = position;

	//f_signalChanged.emit();
}


int Character::getSubPosition() const
{
	return f_subPosition;
}


void Character::setSubPosition( const int pos )
{
	f_subPosition = pos;
}


int Character::getManualPos() const
{
	return f_manualPos;
}


void Character::setManualPos( const int pos )
{
	f_manualPos = pos;
}


int Character::addEffect( Effects::Effect::Pointer effect )
{
	int id = -1;
	EffectEntries::iterator	iter = f_effects.begin();
	EffectEntries::iterator	end  = f_effects.end();

	for( ; iter != end; ++iter )
	{
		if( iter->f_deleted )
		{
			id = iter->f_id;
			iter->f_effect = effect;
			break;
		}
	}

	EffectEntry	entry( (id != -1)? id: f_effects.size(), effect );
	f_effects.push_back( entry );
	id = entry.f_id;

	return id;
}


Effects::Effect::Pointer Character::getEffect( const int id )
{
	return f_effects[id].f_effect;
}


void Character::getEffects( Effects::Effect::List& effects )
{
	effects.clear();

	EffectEntries::iterator	iter = f_effects.begin();
	EffectEntries::iterator	end  = f_effects.end();

	for( ; iter != end; ++iter )
	{
		Effects::Effect::Pointer effect = iter->f_effect;
		Effects::Effect::Pointer newEffect;
		newEffect.reset( new Effects::Effect );
		//
		// Make a copy and give that back out
		//
		*newEffect = *effect;
		effects.push_back( newEffect );
	}
}


void	Character::setEffects( const Effects::Effect::List& effects )
{
	f_effects.clear();

	Effects::Effect::List::const_iterator	iter = effects.begin();
	Effects::Effect::List::const_iterator	end  = effects.end();

	for( auto from : effects )
	{
		Effects::Effect::Pointer to;
		to.reset( new Effects::Effect );
		*to = *from;			// Copy it
		addEffect( to );
	}
}

void Character::removeEffect( const int id )
{
	f_effects[id].f_deleted = true;
}


Health Character::health()
{
#if 0
	// TODO: implement this when abilities are rolled in (<-- I see what you did there...)
	//
	const int conId = f_statMgr->conId();
	Attribute::Value::Pointer conValue = GetStat( conId );
	assert(conValue);
#endif
	Health health = HNormal;

	// Set the health (temporarily) to dead
	//
	auto statMgr     = Application::Manager::Instance().lock()->GetStatMgr().lock();
	auto appSettings = Application::Manager::Instance().lock()->GetAppSettings().lock();
	const int hp = hitpoints();
	const int level = getMod( statMgr->levelId() );
	const int dead_level = appSettings->DeathThreshold()
						 + (appSettings->AlternateDeathRule()? -level: 0)
						 ;
	if( hp == 0 )
	{
		health = Disabled;
	}
	else if( (hp <= dead_level) /*|| (conValue->total() == 0)*/ )
	{
		health = Dead;
	}
	else if( hp < 0 )
	{
		health = f_stabilized? Stabilized: Dying;
	}

	return health;
}


Status Character::status()
{
	Status status;
	//
	switch( f_status )
	{
		case Delayed	: status = Delayed;	break;
		case Readied	: status = Readied;	break;
		default			: status = Normal;	break;
	}
	
	return status;
}


void Character::status( const Status status )
{
	f_status = status;
}


std::string Character::GetStatString( const std::string& health_str )
{
	std::string		stat_str;
	const Health	hl = health();

	switch( hl )
	{
		case HNormal	:	stat_str = health_str			; break;
		case Dead    	:	stat_str = gettext("Dead")    	; break;
		case Dying   	:	stat_str = gettext("Dying")   	; break;
		case Stabilized	:	stat_str = gettext("Stabilized"); break;
		case Disabled	:	stat_str = gettext("Disabled")	; break;
		default			:	assert(0);
	}

	if( hl != Dead )
	{
		switch( f_status )
		{
			case Delayed	: stat_str += ", Delayed";	break;
			case Readied	: stat_str += ", Readied";	break;
			default			: stat_str += "";
		}
	}

	return stat_str;
}


int Character::GetHpPercent()
{
	return (int) (((double) hitpoints() / (double) maxHP()) * 100.0L);
}


Glib::ustring Character::status_string()
{
	const int hp_pct = GetHpPercent();
#if 0
	const int BUFLEN = 16;
	char buf[BUFLEN+1];
	snprintf( buf, BUFLEN, "%d%%", hp_pct );
#else
	std::stringstream ss;
	ss << hp_pct << "%" << std::ends;
	std::string buf = ss.str();
#endif
	//
	return GetStatString( buf );
}


int Character::status_sort()
{
	const int hp_pct = GetHpPercent();

	const Health	hl = health();
	//
	if( hl != HNormal )
	{
		// Force these at the top
		//
		return -hl;
	}

	// Else return the percentage
	//
	return hp_pct;
}


int Character::maxHP()
{
#ifdef OLD_UI
	return f_baseHP;
#else
	// TODO: implement this with Abilities!
	//
	Ability::Pointer a = boost::dynamic_pointer_cast<Ability::Pointer>((static_pointer_cast<Stat::Pointer>(GetStat(Stat::Abilities,conId)));
	assert(a);
	return Common::CalculateHP( a->total(), f_hitDice, f_baseHP, f_tempHP );
#endif
}


Attribute::Value::Pointer Character::GetStat( const molib::mo_name_t id )
{
	Attribute::Value::Pointer value( f_values[id] );
	//
	if( !value )
	{
		// Create the statbase object if it does not already exist...
		//
		auto statMgr = Application::Manager::Instance().lock()->GetStatMgr();
		auto stat    = statMgr.lock()->GetStat( id );
		//
		value = CreateStat( stat );
		f_values[id] = value;
	}
	//
	return value;
}


int Character::getMod( Attribute::Value::Pointer value, const bool with_ability )
{
	assert(value);
	int mod = value->mod();

#if !defined(OLD_UI)
	// TODO: Add this back in when we implement abilities again
	// But, a lot has changed! There are no longer any StatBase::Type(s). And StatBase is now
	// Value, and there are no deriviative classes...
	//
	if( with_ability )
	{
		const int ability_id = stat->abilityId();

		Ability* ability = dynamic_cast<Ability*>(static_cast<StatBase*>(GetStat(Stat::Abilities, ability_id )));
		if( ability )
		{
			mod += Common::StatToMod( ability->total() );
		}

		SaveStat* save = dynamic_cast<SaveStat*>(static_cast<StatBase*>(stat));
		if( save )
		{
			mod += save->base();
		}

		Skill* skill = dynamic_cast<Skill*>(static_cast<StatBase*>(stat));
		if( skill )
		{
			mod += skill->ranks();
		}
	}
#endif	// !defined(OLD_UI)

	return mod;
}


void Character::setMod( Attribute::Value::Pointer stat, const int mod )
{
	assert(stat);
	stat->mod(mod);
}

int Character::getRoll( Attribute::Value::Pointer stat )
{
	assert(stat);
	return stat->roll(); //+ getMod( stat, true /*with_ability*/ );
}


void Character::setRoll( Attribute::Value::Pointer stat, const int roll )
{
	assert(stat);
	stat->roll( roll );
}


molib::moWCString Character::getNotes( Attribute::Value::Pointer stat )
{
	assert(stat);
	return stat->notes();
}


void Character::setNotes( Attribute::Value::Pointer stat, const char* notes )
{
	assert(stat);
	stat->notes( notes );
}

	
}
// namespace Combatant


// vi: ts=4 sw=4 syntax=cpp.doxygen

