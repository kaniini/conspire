/* this file is auto generated, edit textevents.in instead! */

const struct text_event te[] = {

{"Add Notify", 1, 
"%C27*$t%C27$1 added to notify list."},

{"Ban List", 4, 
"%C27*$t%C27$1 Banlist: %C3$4 %C4$2 %C5$3%O"},

{"Beep", 0, 
""},

{"Change Nick", 2, 
"%C27*$t%C27$1 is now known as $2"},

{"Channel Action", 3, 
"%C13*%O$t$1 $3%O"},

{"Channel Action Hilight", 3, 
"%C13*%O$t%C8%B$1%B%O $3%O"},

{"Channel Creation", 2, 
"%C27*$t%C27Channel $1 created on $2"},

{"Channel Join Error", 2, 
"%C27*$t%C27Cannot join%C11 %B$1%O ($2)."},

{"Channel List", 0, 
""},

{"Channel List Entry", 3, 
"%C27*$t%O%B$1%O%C27/%O$2%C27: %O$3%O"},

{"Channel Message", 4, 
"%C2%H<%H$1%H>%H%O$t$4%O"},

{"Channel Mode Generic", 4, 
"%C27*$t%C27$1 sets mode $2$3 $4"},

{"Channel Modes", 2, 
"%C27*$t%C27Channel $1 modes: $2"},

{"Channel Msg Hilight", 4, 
"%C8%B%H<%H$1%H>%H%O$t$4%O"},

{"Channel Notice", 3, 
"%C12-%C13$1/$2%C12-%O$t$3%O"},

{"Connected", 0, 
"%C27*$t%C27Connected. Now logging in.."},

{"Connecting", 3, 
"%C27*$t%C27Connecting to %C11$1 %C14(%C11$2%C14)%C11 port %C11$3%C.."},

{"Connection Failed", 1, 
"%C27*$t%C27Connection failed. Error: $1"},

{"CTCP Generic", 2, 
"%C27*$t%C27Received a CTCP $1 from $2"},

{"CTCP Generic to Channel", 3, 
"%C27*$t%C27Received a CTCP $1 from $2 (to $3)"},

{"CTCP Reply Generic", 3, 
"%C27*$t%C27Received a CTCP reply $2 from $1: $3"},

{"CTCP Send", 2, 
"%C3>%O$1%C3<%O$tCTCP $2%O"},

{"DCC CHAT Abort", 1, 
"%C27*$t%C27DCC CHAT to %C11$1%O aborted."},

{"DCC CHAT Connect", 2, 
"%C27*$t%C27DCC CHAT connection established to %C11$1 %C14[%O$2%C14]%O"},

{"DCC CHAT Failed", 4, 
"%C27*$t%C27DCC CHAT failed. Connection to $1 %C14[%O$2:$3%C14]%O lost."},

{"DCC CHAT Offer", 1, 
"%C27*$t%C27Received a DCC CHAT offer from $1"},

{"DCC CHAT Offering", 1, 
"%C27*$t%C27Offering DCC CHAT to $1"},

{"DCC CHAT Reoffer", 1, 
"%C27*$t%C27Already offering CHAT to $1"},

{"DCC Conection Failed", 3, 
"%C27*$t%C27DCC $1 connect attempt to %C11$2%O failed (err=$3)."},

{"DCC Generic Offer", 2, 
"%C27*$t%C27Received '$1%O' from $2"},

{"DCC Header", 0, 
"%C24,18 Type  To/From    Status  Size    Pos     File         "},

{"DCC Malformed", 2, 
"%C27*$t%C27Received a malformed DCC request from %C11$1%O.%010%C27*$t%C27Contents of packet: $2"},

{"DCC Offer", 3, 
"%C27*$t%C27Offering %C11$1 %Cto %C11$2%O"},

{"DCC Offer Not Valid", 0, 
"%C27*$t%C27No such DCC offer."},

{"DCC RECV Abort", 2, 
"%C27*$t%C27DCC RECV %C11$2%O to %C11$1%O aborted."},

{"DCC RECV Complete", 4, 
"%C27*$t%C27DCC RECV %C11$1%O from %C11$3%O complete %C14[%C11$4%O cps%C14]%O."},

{"DCC RECV Connect", 3, 
"%C27*$t%C27DCC RECV connection established to %C11$1 %C14[%O$2%C14]%O"},

{"DCC RECV Failed", 4, 
"%C27*$t%C27DCC RECV $1 ($2) failed. Connection to $3 lost."},

{"DCC RECV File Open Error", 2, 
"%C27*$t%C27DCC RECV: Cannot open $1 for writing ($2)."},

{"DCC Rename", 2, 
"%C27*$t%C27The file %C11$1%C already exists, saving it as %C11$2%O instead."},

{"DCC RESUME Request", 3, 
"%C27*$t%C27%C11$1 %Chas requested to resume %C11$2 %Cfrom %C11$3%C."},

{"DCC SEND Abort", 2, 
"%C27*$t%C27DCC SEND %C11$2%O to %C11$1%O aborted."},

{"DCC SEND Complete", 3, 
"%C27*$t%C27DCC SEND %C11$1%O to %C11$2%O complete %C14[%C11$3%O cps%C14]%O."},

{"DCC SEND Connect", 3, 
"%C27*$t%C27DCC SEND connection established to %C11$1 %C14[%O$2%C14]%O"},

{"DCC SEND Failed", 3, 
"%C27*$t%C27DCC SEND %C11$1%O failed. Connection to %C11$2%O lost."},

{"DCC SEND Offer", 3, 
"%C27*$t%C27You have offered %C11$3 %C14(%C11$1 %C27bytes%C14)%C27 to %C11$2%C27."},

{"DCC Stall", 3, 
"%C27*$t%C27DCC $1 %C11$2 %Cto %C11$3 %Cstalled - aborting."},

{"DCC Timeout", 3, 
"%C27*$t%C27DCC $1 %C11$2 %Cto %C11$3 %Ctimed out - aborting."},

{"Delete Notify", 1, 
"%C27*$t%C27$1 deleted from notify list."},

{"Disconnected", 1, 
"%C27*$t%C27Disconnected ($1)."},

{"Found IP", 1, 
"%C27*$t%C27The IRC server is reporting your IP as $1."},

{"Generic Message", 2, 
"$1$t$2"},

{"Ignore Add", 1, 
"%C27*$t%C27$1 added to ignore list."},

{"Ignore Changed", 1, 
"%C27*$t%C27Ignore on $1 has been changed."},

{"Ignore Footer", 0, 
"%C27*$t%C27End of ignore list."},

{"Ignore Header", 0, 
"%C27*$t%C27List of current ignores:"},

{"Ignore Remove", 1, 
"%C27*$t%C27$1 removed from ignore list."},

{"Ignorelist Empty", 0, 
"%C27*$t%C27Ignore list is empty."},

{"Ignorelist Entry", 0, 
"%C27*$t%C27$1"},

{"Invited", 3, 
"%C27*$t%C27You have been invited to $1 by $2 ($3)."},

{"Join", 3, 
"%C27*$t%C27$1 ($3) has joined $2"},

{"Kick", 4, 
"%C27*$t%C27$1 has kicked $2 from $3: $4"},

{"Killed", 2, 
"%C27*$t%C27You have been killed by $1: $2"},

{"Message Send", 2, 
"%C3>%O$1%C3<%O$t$2%O"},

{"Motd", 1, 
"%C27*$t%O$1"},

{"MOTD Skipped", 0, 
"%C27*$t%C27MOTD Skipped."},

{"Nick Clash", 2, 
"%C27*$t%O$1 already in use. Retrying with $2.."},

{"Nick Failed", 0, 
"%C27*$t%ONickname already in use. Use /NICK to try another."},

{"No DCC", 0, 
"%C27*$t%C27No such DCC."},

{"No Running Process", 0, 
"%C27*$t%C27No process is currently running"},

{"Notice", 2, 
"%C12-%C13$1%C12-%O$t$2%O"},

{"Notice Send", 2, 
"%C3>%O$1%C3<%O$t$2%O"},

{"Notify Empty", 0, 
"%C27*$t%C27Notify list is empty."},

{"Notify Header", 0, 
"%C24,18 %B  Notify List                           "},

{"Notify Number", 1, 
"%C27*$t%C27$1 users in notify list."},

{"Notify Offline", 2, 
"%C27*$t%C27Notify: $1 is offline ($2)."},

{"Notify Online", 2, 
"%C27*$t%C27Notify: $1 is online ($2)."},

{"Open Dialog", 0, 
""},

{"Part", 3, 
"%C27*$t%C27$1 %C14(%O$2%C14)%C has left $3"},

{"Part with Reason", 4, 
"%C27*$t%C27$1 %C14(%O$2%C14)%C has left $3: $4"},

{"Ping Reply", 2, 
"%C27*$t%C27Ping reply from $1: $2 second(s)"},

{"Ping Timeout", 1, 
"%C27*$t%C27No ping reply for $1 seconds, disconnecting."},

{"Private Message", 3, 
"%C12*%C13$1%C12*$t%O$2%O"},

{"Private Message to Dialog", 3, 
"%C2%H<%H$1%H>%H%O$t$2%O"},

{"Process Already Running", 0, 
"%C27*$t%C27A process is already running"},

{"Quit", 3, 
"%C27*$t%C27$1 ($3) has quit: $2"},

{"Query Action", 3, 
"%C13*%O$t$1 $3%O"},

{"Query Action Hilight", 3, 
"%C13*%O$t%C8%B$1%B%O $3%O"},

{"Raw Modes", 2, 
"%C27*$t%C27$1 sets mode: $2"},

{"Receive Wallops", 2, 
"%C12-%C13$1/Wallops%C12-%O$t$2%O"},

{"Resolving User", 2, 
"%C27*$t%C27Looking up IP number for%C11 $1%O.."},

{"Server Connected", 0, 
"%C27*$t%C27Connected."},

{"Server Error", 1, 
"%C27*$t%C27$1%O"},

{"Server Lookup", 1, 
"%C27*$t%C27Looking up %C11$1%C.."},

{"Server Notice", 2, 
"%C27*%O$t$1"},

{"Server Text", 2, 
"%C27*$t%O$1%O"},

{"Stop Connection", 1, 
"%C27*$t%C27Stopped previous connection attempt (pid=$1)"},

{"Topic", 2, 
"%C27*$t%C27Topic for $1 is $2"},

{"Topic Change", 3, 
"%C27*$t%C27$1 has changed the topic to: $2"},

{"Topic Creation", 3, 
"%C27*$t%C27Topic for $1 set by $2 at $3"},

{"Unknown Host", 0, 
"%C27*$t%C27Unknown host. Maybe you misspelled it?"},

{"Users On Channel", 2, 
"%C27*$t%C27%C11Users on $1:%C $2"},

{"WhoIs Authenticated", 3, 
"%C22*%O$t%C28[%O$1%C28] %O$2%C27 $3"},

{"WhoIs Away Line", 2, 
"%C27*$t%C27%C12[%O$1%C12] is away: $2"},

{"WhoIs Channel/Oper Line", 2, 
"%C27*$t%C27%C12[%O$1%C12]%C $2"},

{"WhoIs Oper Line", 2, 
"%C27*$t%C27%C12[%O$1%C12]%C $2"},

{"WhoIs End", 1, 
"%C27*$t%C27%C12[%O$1%C12] %CEnd of WHOIS list."},

{"WhoIs Identified", 2, 
"%C27*$t%C27%C12[%O$1%C12] %C$2"},

{"WhoIs Idle Line", 2, 
"%C27*$t%C27%C12[%O$1%C12]%O idle %C11$2%O"},

{"WhoIs Idle Line with Signon", 3, 
"%C27*$t%C27%C12[%O$1%C12]%O idle %C11$2%O, signon: %C11$3%O"},

{"WhoIs Name Line", 4, 
"%C27*$t%C27%C12[%O$1%C12] %C14(%O$2@$3%C14) %O: $4%O"},

{"WhoIs Real Host", 4, 
"%C27*$t%C27%C12[%O$1%C12] %Oreal user@host %C11$2%O, real IP %C11$3%O"},

{"WhoIs Server Line", 2, 
"%C27*$t%C27%C12[%O$1%C12]%O $2"},

{"WhoIs Special", 3, 
"%C22*%O$t%C28[%O$1%C28]%O $2"},

{"You Join", 3, 
"%C27*$t%C27Now talking on %C11$2%O"},

{"You Kicked", 4, 
"%C27*$t%C27You have been kicked from $2 by $3: $4"},

{"You Part", 3, 
"%C27*$t%C27You have left channel $3"},

{"You Part with Reason", 4, 
"%C27*$t%C27You have left channel $3: $4"},

{"Your Action", 3, 
"%C18*$t$1%O $2"},

{"Your Invitation", 3, 
"%C27*$t%C27You've invited %C11$1%C to %C11$2%C (%C11$3%C)"},

{"Your Message", 4, 
"%C31%H<%H$1%H>%H%O$t$2%O"},

{"Your Nick Changing", 2, 
"%C27*$t%C27You are now known as $2"},

{"Netsplit", 3, 
"%C23*%O$t%C23Netsplit: $1 <-> $2, quits: $3"},

{"Authenticated to Account", 1, 
"%C23*%O$t%C23You have successfully authenticated to account '$1'."},

{"Plugin Loaded", 2, 
"%C23*%O$t%C23Plugin $1 version $2 successfully loaded."},

{"Plugin Unloaded", 1, 
"%C23*%O$t%C23Plugin $1 successfully unloaded."},

{"Plugin Error", 2, 
"%C23*%O$t%C23Couldn't load plugin $1: $2"},
};
