#if !defined _attdef_header_incl
	#error "Module's header file include is not allowed here, include it in the correct section of the 'legacy/modules.inc' file."
#endif

#if defined _attdef_mod_match
	#endinput
#endif
#define _attdef_mod_match

/*
	Round/match related
*/

#include "modules/fields.inc"
#include "modules/this_core.inc"

// Round never ends if this is not commented. Useful for testing. Make sure it's commented on release though.
//#define ROUND_NEVER_END

// Teams definition
#define NON						0 // Team nothing, used for when the player join the server
#define ATTACKER				1 // Attacker team
#define DEFENDER				2 // Defender team
#define REFEREE					3 // Referee team
#define ATTACKER_SUB			4 // Attacker sub team
#define DEFENDER_SUB			5 // Defender sub team

// Game Types
#define BASE					0 // A game of 2 teams, one defends a checkpoint while the other tries to capture it
#define ARENA					1 // A game of 2 teams, each team has to eliminate the other to win

// Const
#define MAX_ZONE_LIMIT_WARNINGS 5 // How many warnings should a player get before we remove him for going out of boundries?

new bool: AlreadyEndingRound	 = false;
new bool: AllowStartBase		 = true;   // When this is set to true, players can start other rounds. False is the other way around
new bool: AllowSync				 = true;   // Mainly to disallow SyncPlayer for players in a starting round
new bool: PreMatchResultsShowing = false;  // True means the server is processing pre-match results. False means process is not active
new bool: AutoBal				 = true;   // When set to true, teams will be auto-balanced (when war mode is off). False is the other way around
new bool: AutoPause				 = true;   // When set to true, any active rounds will be paused when a player disconnects
new bool: BaseStarted			 = false;  // True means there's a base in progress
new bool: ArenaStarted			 = false;  // True means there's an arena in progress
new bool: RoundPaused			 = false;  // True means the round is paused. False means it's running.
new bool: RoundUnpausing		 = false;  // True means the round is being unpaused at the moment. False means it's running or still paused.
new bool: WarMode				 = false;  // If match/war mode is on
new bool: WarModeEnding			 = false;  // If match/war mode is on
new bool: MatchEnded			 = false;  // This is set to true when a match is done
new bool: RCArena				 = false;  // RC Battlefield arena, a new simple type of minigames
new bool: TeamHelp[MAX_TEAMS];			   // Whether a team is asking for help or not
new bool: IsBasePlayed[MAX_BASES];		   // Whether a base is played or not
new bool: IsArenaPlayed[MAX_ARENAS];	   // Whether an arena is played or not
new PausePressed;						   // A tick count when the last time pause command (or button) was used (or pressed)
new PreStartRoundTimer	= 0;			   // Timer used to start the countdown for a round start
new ViewRoundTimer		= 0;			   // Timer used every second to view a round when started
new GameLoop			= 0;			   // The ID of the timer that handles everything about the current active round
new PauseStopWatchTimer = 0;			   // The ID of the timer that counts up on each round pause
new PauseTimeElapsed;					   // Time elapsed since round pause (minutes and seconds)
new TotalPausesInRound;					   // How many times the round has been paused in one round
new TipTimerDuringPause = 0;			   // A timer which is used to send tips after a certain time since a round pause
new Iterator: FrozenCars<MAX_VEHICLES>;	   // Temporarily contains IDs of vehicles when round is paused
new FrozenCarTimers[MAX_VEHICLES];		   // Contains IDs of timers for FrozenCars
new Iterator: PlayersInRound<MAX_PLAYERS>; // The IDs of players in round (alive/playing) are to be stored in this array
new ViewRoundCountdown;					   // This is used when viewing a round to players as a countdown
new Float: ViewBaseCamPos[3];			   // Current position of base viewer camera
new PauseCountdown;						   // Countdown of pause
new CurrentCPTime;						   // Current count of checkpoint
new TeamCapturingCP = NON;				   // What team is capturing the checkpoint (used in arenas)
new AttHpTimer;							   // Holds the ID of the timer that hides attacker taken-damage info after a specific time
new DefHpTimer;							   // Holds the ID of the timer that hides defender taken-damage info after a specific time
new UnpauseTimer;						   // Holds the ID of the unpause round timer
new Current = -1;						   // Current round ID. -1 means there's no active round at the moment.
new PlayersAlive[MAX_TEAMS];			   // How many players are alive in each team
new PlayersDead[MAX_TEAMS];				   // How many players are dead in each team
new TeamHP[MAX_TEAMS];					   // How much HP does a team have
new TeamName[MAX_TEAMS][24];			   // Used to hold the team name in WarMode
new TeamScore[MAX_TEAMS];				   // Used to the team round score in Warmode
new Float: TempDamage[MAX_TEAMS];		   // Temporary taken-damage of each team
new ServerLastPlayed;					   // Holds the ID of the last played round
new ServerLastPlayedType;				   // Remembers the type of the last played round (base, round ..etc)
new PlayersInCP;						   // How many players are capturing the checkpoint
new RoundSeconds;						   // Round seconds (countdown)
new GameType;							   // Currently played round game type
new TotalRounds;						   // Total rounds
new CurrentRound;						   // Current round count (i.e how many rounds have been played)
new Float: VehicleVelc[MAX_VEHICLES][3];   // Holds vehicle velocity info. This is used when a round gets unpaused, vehicles are set to their original speed
new CPZone;								   // Holds the ID of an CP gangzone
new ArenaZone;							   // Holds the ID of an arena gangzone
new ElapsedTime;						   // Holds the time elapsed since a round was started
new bool: TeamHasLeader[MAX_TEAMS];		   // Whether a team has leader or not
new TeamLeader[MAX_TEAMS];				   // Holds the ID of the current team leader
new TeamDeathIconTimer[MAX_TEAMS];		   // Holds the ID of team death map icon destroyer timer
new LastMatchEndTime;

// String for round results
new AttList[256];
new AttKills[256];
new AttDeaths[256];
new AttPlayed[256];
new AttAcc[256];
new AttDamage[256];

new DefList[256];
new DefKills[256];
new DefDeaths[256];
new DefPlayed[256];
new DefAcc[256];
new DefDamage[256];

// Match recording
enum round_record {
	round__ID,
	round__type, //1 base | 2 arena
	bool: round__completed
}

new MatchRoundsRecord[101][round_record];
new MatchRoundsStarted = 0;
