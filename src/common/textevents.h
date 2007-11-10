/* this file is auto generated, edit textevents.in instead! */

const struct text_event te[] = {

{"Add Notify", pevt_generic_nick_help, 1, 
N_("%C27*$t%C27$1 added to notify list.")},

{"Ban List", pevt_banlist_help, 4, 
N_("%C27*$t%C27$1 Banlist: %C3$4 %C4$2 %C5$3%O")},

{"Banned", pevt_generic_channel_help, 1, 
N_("%C27*$t%C27Cannot join%C11 %B$1 %O(You are banned).")},

{"Beep", pevt_generic_none_help, 0, 
N_("")},

{"Change Nick", pevt_changenick_help, 2, 
N_("%C27*$t%C27$1 is now known as $2")},

{"Channel Action", pevt_chanaction_help, 2, 
N_("%C13*%O$t$1 $2%O")},

{"Channel Action Hilight", pevt_chanaction_help, 2, 
N_("%C13*%O$t%C8%B$1%B%O $2%O")},

{"Channel Ban", pevt_chanban_help, 2, 
N_("%C27*$t%C27$1 sets ban on $2")},

{"Channel Creation", pevt_chandate_help, 2, 
N_("%C27*$t%C27Channel $1 created on $2")},

{"Channel DeHalfOp", pevt_chandehop_help, 2, 
N_("%C27*$t%C27$1 removes channel half-operator status from $2")},

{"Channel DeOp", pevt_chandeop_help, 2, 
N_("%C27*$t%C27$1 removes channel operator status from $2")},

{"Channel DeVoice", pevt_chandevoice_help, 2, 
N_("%C27*$t%C27$1 removes voice from $2")},

{"Channel Exempt", pevt_chanexempt_help, 2, 
N_("%C27*$t%C27$1 sets exempt on $2")},

{"Channel Half-Operator", pevt_chanhop_help, 2, 
N_("%C27*$t%C27$1 gives channel half-operator status to $2")},

{"Channel INVITE", pevt_chaninvite_help, 2, 
N_("%C27*$t%C27$1 sets invite on $2")},

{"Channel List", pevt_generic_none_help, 0, 
N_("%UChannel          Users   Topic%O")},

{"Channel Message", pevt_chanmsg_help, 4, 
N_("%C2%H<%H$1%H>%H%O$t$2%O")},

{"Channel Mode Generic", pevt_chanmodegen_help, 4, 
N_("%C27*$t%C27$1 sets mode $2$3 $4")},

{"Channel Modes", pevt_chanmodes_help, 2, 
N_("%C27*$t%C27Channel $1 modes: $2")},

{"Channel Msg Hilight", pevt_chanmsg_help, 4, 
N_("%C8%B%H<%H$1%H>%H%O$t$2%O")},

{"Channel Notice", pevt_channotice_help, 3, 
N_("%C12-%C13$1/$2%C12-%O$t$3%O")},

{"Channel Operator", pevt_chanop_help, 2, 
N_("%C27*$t%C27$1 gives channel operator status to $2")},

{"Channel Remove Exempt", pevt_chanrmexempt_help, 2, 
N_("%C27*$t%C27$1 removes exempt on $2")},

{"Channel Remove Invite", pevt_chanrminvite_help, 2, 
N_("%C27*$t%C27$1 removes invite on $2")},

{"Channel Remove Keyword", pevt_chanrmkey_help, 1, 
N_("%C27*$t%C27$1 removes channel keyword")},

{"Channel Remove Limit", pevt_chanrmlimit_help, 1, 
N_("%C27*$t%C27$1 removes user limit")},

{"Channel Set Key", pevt_chansetkey_help, 2, 
N_("%C27*$t%C27$1 sets channel keyword to $2")},

{"Channel Set Limit", pevt_chansetlimit_help, 2, 
N_("%C27*$t%C27$1 sets channel limit to $2")},

{"Channel UnBan", pevt_chanunban_help, 2, 
N_("%C27*$t%C27$1 removes ban on $2")},

{"Channel Voice", pevt_chanvoice_help, 2, 
N_("%C27*$t%C27$1 gives voice to $2")},

{"Connected", pevt_generic_none_help, 0, 
N_("%C27*$t%C27Connected. Now logging in..")},

{"Connecting", pevt_connect_help, 3, 
N_("%C27*$t%C27Connecting to %C11$1 %C14(%C11$2%C14)%C11 port %C11$3%C..")},

{"Connection Failed", pevt_connfail_help, 1, 
N_("%C27*$t%C27Connection failed. Error: $1")},

{"CTCP Generic", pevt_ctcpgen_help, 2, 
N_("%C27*$t%C27Received a CTCP $1 from $2")},

{"CTCP Generic to Channel", pevt_ctcpgenc_help, 3, 
N_("%C27*$t%C27Received a CTCP $1 from $2 (to $3)")},

{"CTCP Send", pevt_ctcpsend_help, 2, 
N_("%C3>%O$1%C3<%O$tCTCP $2%O")},

{"CTCP Sound", pevt_ctcpsnd_help, 2, 
N_("%C27*$t%C27Received a CTCP Sound $1 from $2")},

{"CTCP Sound to Channel", pevt_ctcpsnd_help, 3, 
N_("%C22*%O$tReceived a CTCP Sound $1 from $2 (to $3)")},

{"DCC CHAT Abort", pevt_dccchatabort_help, 1, 
N_("%C27*$t%C27DCC CHAT to %C11$1%O aborted.")},

{"DCC CHAT Connect", pevt_dccchatcon_help, 2, 
N_("%C27*$t%C27DCC CHAT connection established to %C11$1 %C14[%O$2%C14]%O")},

{"DCC CHAT Failed", pevt_dccchaterr_help, 4, 
N_("%C27*$t%C27DCC CHAT failed. Connection to $1 %C14[%O$2:$3%C14]%O lost.")},

{"DCC CHAT Offer", pevt_generic_nick_help, 1, 
N_("%C27*$t%C27Received a DCC CHAT offer from $1")},

{"DCC CHAT Offering", pevt_generic_nick_help, 1, 
N_("%C27*$t%C27Offering DCC CHAT to $1")},

{"DCC CHAT Reoffer", pevt_generic_nick_help, 1, 
N_("%C27*$t%C27Already offering CHAT to $1")},

{"DCC Conection Failed", pevt_dccconfail_help, 3, 
N_("%C27*$t%C27DCC $1 connect attempt to %C11$2%O failed (err=$3).")},

{"DCC Generic Offer", pevt_dccgenericoffer_help, 2, 
N_("%C27*$t%C27Received '$1%O' from $2")},

{"DCC Header", pevt_generic_none_help, 0, 
N_("%C24,18 Type  To/From    Status  Size    Pos     File         ")},

{"DCC Malformed", pevt_malformed_help, 2, 
N_("%C27*$t%C27Received a malformed DCC request from %C11$1%O.%010%C27*$t%C27Contents of packet: $2")},

{"DCC Offer", pevt_dccoffer_help, 3, 
N_("%C27*$t%C27Offering %C11$1 %Cto %C11$2%O")},

{"DCC Offer Not Valid", pevt_generic_none_help, 0, 
N_("%C27*$t%C27No such DCC offer.")},

{"DCC RECV Abort", pevt_dccfileabort_help, 2, 
N_("%C27*$t%C27DCC RECV %C11$2%O to %C11$1%O aborted.")},

{"DCC RECV Complete", pevt_dccrecvcomp_help, 4, 
N_("%C27*$t%C27DCC RECV %C11$1%O from %C11$3%O complete %C14[%C11$4%O cps%C14]%O.")},

{"DCC RECV Connect", pevt_dcccon_help, 3, 
N_("%C27*$t%C27DCC RECV connection established to %C11$1 %C14[%O$2%C14]%O")},

{"DCC RECV Failed", pevt_dccrecverr_help, 4, 
N_("%C27*$t%C27DCC RECV $1 ($2) failed. Connection to $3 lost.")},

{"DCC RECV File Open Error", pevt_generic_file_help, 2, 
N_("%C27*$t%C27DCC RECV: Cannot open $1 for writing ($2).")},

{"DCC Rename", pevt_dccrename_help, 2, 
N_("%C27*$t%C27The file %C11$1%C already exists, saving it as %C11$2%O instead.")},

{"DCC RESUME Request", pevt_dccresumeoffer_help, 3, 
N_("%C27*$t%C27%C11$1 %Chas requested to resume %C11$2 %Cfrom %C11$3%C.")},

{"DCC SEND Abort", pevt_dccfileabort_help, 2, 
N_("%C27*$t%C27DCC SEND %C11$2%O to %C11$1%O aborted.")},

{"DCC SEND Complete", pevt_dccsendcomp_help, 3, 
N_("%C27*$t%C27DCC SEND %C11$1%O to %C11$2%O complete %C14[%C11$3%O cps%C14]%O.")},

{"DCC SEND Connect", pevt_dcccon_help, 3, 
N_("%C27*$t%C27DCC SEND connection established to %C11$1 %C14[%O$2%C14]%O")},

{"DCC SEND Failed", pevt_dccsendfail_help, 3, 
N_("%C27*$t%C27DCC SEND %C11$1%O failed. Connection to %C11$2%O lost.")},

{"DCC SEND Offer", pevt_dccsendoffer_help, 4, 
N_("%C27*$t%C27%C11$1 %Chas offered %C11$2 %C(%C11$3 %Cbytes)")},

{"DCC Stall", pevt_dccstall_help, 3, 
N_("%C27*$t%C27DCC $1 %C11$2 %Cto %C11$3 %Cstalled - aborting.")},

{"DCC Timeout", pevt_dccstall_help, 3, 
N_("%C27*$t%C27DCC $1 %C11$2 %Cto %C11$3 %Ctimed out - aborting.")},

{"Delete Notify", pevt_generic_nick_help, 1, 
N_("%C27*$t%C27$1 deleted from notify list.")},

{"Disconnected", pevt_discon_help, 1, 
N_("%C27*$t%C27Disconnected ($1).")},

{"Found IP", pevt_foundip_help, 1, 
N_("%C27*$t%C27Found your IP: [$1]")},

{"Generic Message", pevt_genmsg_help, 2, 
N_("$1$t$2")},

{"Ignore Add", pevt_ignoreaddremove_help, 1, 
N_("%O%C11$1%O added to ignore list.")},

{"Ignore Changed", pevt_ignoreaddremove_help, 1, 
N_("Ignore on %C11$1%O changed.")},

{"Ignore Footer", pevt_generic_none_help, 0, 
N_("%C24,18                                                              ")},

{"Ignore Header", pevt_generic_none_help, 0, 
N_("%C24,18 Hostmask                  PRIV NOTI CHAN CTCP DCC  INVI UNIG ")},

{"Ignore Remove", pevt_ignoreaddremove_help, 1, 
N_("%O%C11$1%O removed from ignore list.")},

{"Ignorelist Empty", pevt_generic_none_help, 0, 
N_("  Ignore list is empty.")},

{"Invite", pevt_generic_channel_help, 1, 
N_("%C27*$t%C27Cannot join%C11 %B$1 %O(Channel is invite only).")},

{"Invited", pevt_invited_help, 3, 
N_("%C27*$t%C27You have been invited to %C11$1%C by %C11$2%C (%C11$3%C)")},

{"Join", pevt_join_help, 3, 
N_("-%C10-%C11>%O$t%B$1%B %C14(%C10$3%C14)%C has joined $2")},

{"Keyword", pevt_generic_channel_help, 1, 
N_("%C27*$t%C27Cannot join%C11 %B$1 %O(Requires keyword).")},

{"Kick", pevt_kick_help, 4, 
N_("%C27*$t%C27$1 has kicked $2 from $3: $4")},

{"Killed", pevt_kill_help, 2, 
N_("%C27*$t%C27You have been killed by $1: $2")},

{"Message Send", pevt_ctcpsend_help, 2, 
N_("%C3>%O$1%C3<%O$t$2%O")},

{"Motd", pevt_servertext_help, 1, 
N_("%C27*$t%O$1")},

{"MOTD Skipped", pevt_generic_none_help, 0, 
N_("%C27*$t%C27MOTD Skipped.")},

{"Nick Clash", pevt_nickclash_help, 2, 
N_("%C27*$t%O$1 already in use. Retrying with $2..")},

{"Nick Failed", pevt_generic_none_help, 0, 
N_("%C27*$t%ONickname already in use. Use /NICK to try another.")},

{"No DCC", pevt_generic_none_help, 0, 
N_("%C27*$t%C27No such DCC.")},

{"No Running Process", pevt_generic_none_help, 0, 
N_("%C27*$t%C27No process is currently running")},

{"Notice", pevt_notice_help, 2, 
N_("%C12-%C13$1%C12-%O$t$2%O")},

{"Notice Send", pevt_ctcpsend_help, 2, 
N_("%C3>%O$1%C3<%O$t$2%O")},

{"Notify Empty", pevt_generic_none_help, 0, 
N_("%C27*$t%C27Notify list is empty.")},

{"Notify Header", pevt_generic_none_help, 0, 
N_("%C24,18 %B  Notify List                           ")},

{"Notify Number", pevt_notifynumber_help, 1, 
N_("%C27*$t%C27$1 users in notify list.")},

{"Notify Offline", pevt_generic_nick_help, 2, 
N_("%C27*$t%C27Notify: $1 is offline ($2).")},

{"Notify Online", pevt_generic_nick_help, 2, 
N_("%C27*$t%C27Notify: $1 is online ($2).")},

{"Open Dialog", pevt_generic_none_help, 0, 
N_("")},

{"Part", pevt_part_help, 3, 
N_("%C27*$t%C27$1 %C14(%O$2%C14)%C has left $3")},

{"Part with Reason", pevt_partreason_help, 4, 
N_("%C27*$t%C27$1 %C14(%O$2%C14)%C has left $3: $4")},

{"Ping Reply", pevt_pingrep_help, 2, 
N_("%C27*$t%C27Ping reply from $1: $2 second(s)")},

{"Ping Timeout", pevt_pingtimeout_help, 1, 
N_("%C27*$t%C27No ping reply for $1 seconds, disconnecting.")},

{"Private Message", pevt_privmsg_help, 3, 
N_("%C12*%C13$1%C12*$t%O$2%O")},

{"Private Message to Dialog", pevt_privmsg_help, 3, 
N_("%C2%H<%H$1%H>%H%O$t$2%O")},

{"Process Already Running", pevt_generic_none_help, 0, 
N_("%C27*$t%C27A process is already running")},

{"Quit", pevt_quit_help, 3, 
N_("%C27*$t%C27$1 ($3) has quit: $2")},

{"Raw Modes", pevt_rawmodes_help, 2, 
N_("%C27*$t%C27$1 sets mode: $2")},

{"Receive Wallops", pevt_privmsg_help, 2, 
N_("%C12-%C13$1/Wallops%C12-%O$t$2%O")},

{"Resolving User", pevt_resolvinguser_help, 2, 
N_("%C27*$t%C27Looking up IP number for%C11 $1%O..")},

{"Server Connected", pevt_generic_none_help, 0, 
N_("%C27*$t%C27Connected.")},

{"Server Error", pevt_servererror_help, 1, 
N_("%C27*$t%C27$1%O")},

{"Server Lookup", pevt_serverlookup_help, 1, 
N_("%C27*$t%C27Looking up %C11$1%C..")},

{"Server Notice", pevt_servertext_help, 2, 
N_("%C27*%O$t$1")},

{"Server Text", pevt_servertext_help, 2, 
N_("%C27*$t%O$1%O")},

{"Stop Connection", pevt_sconnect_help, 1, 
N_("%C27*$t%C27Stopped previous connection attempt (pid=$1)")},

{"Topic", pevt_topic_help, 2, 
N_("%C27*$t%C27Topic for %C11$1%C is %C11$2%O")},

{"Topic Change", pevt_newtopic_help, 3, 
N_("%C27*$t%C27$1 has changed the topic to: $2%O")},

{"Topic Creation", pevt_topicdate_help, 3, 
N_("%C27*$t%C27Topic for %C11$1%C set by %C11$2%C at %C11$3%O")},

{"Unknown Host", pevt_generic_none_help, 0, 
N_("%C27*$t%C27Unknown host. Maybe you misspelled it?")},

{"User Limit", pevt_generic_channel_help, 1, 
N_("%C27*$t%C27Cannot join%C11 %B$1 %O(User limit reached).")},

{"Users On Channel", pevt_usersonchan_help, 2, 
N_("%C27*$t%C27%C11Users on $1:%C $2")},

{"WhoIs Authenticated", pevt_whois_auth_help, 3, 
N_("%C22*%O$t%C28[%O$1%C28] %O$2%C27 $3")},

{"WhoIs Away Line", pevt_whois_away_help, 2, 
N_("%C27*$t%C27%C12[%O$1%C12] is away: $2")},

{"WhoIs Channel/Oper Line", pevt_whois_channels_help, 2, 
N_("%C27*$t%C27%C12[%O$1%C12]%C $2")},

{"WhoIs Oper Line", pevt_whois_oper_help, 2, 
N_("%C27*$t%C27%C12[%O$1%C12]%C $2")},

{"WhoIs End", pevt_whois_end_help, 1, 
N_("%C27*$t%C27%C12[%O$1%C12] %CEnd of WHOIS list.")},

{"WhoIs Identified", pevt_whoisid_help, 2, 
N_("%C27*$t%C27%C12[%O$1%C12] %C$2")},

{"WhoIs Idle Line", pevt_whois_idle_help, 2, 
N_("%C27*$t%C27%C12[%O$1%C12]%O idle %C11$2%O")},

{"WhoIs Idle Line with Signon", pevt_whois_idle_signon_help, 3, 
N_("%C27*$t%C27%C12[%O$1%C12]%O idle %C11$2%O, signon: %C11$3%O")},

{"WhoIs Name Line", pevt_whois_name_help, 4, 
N_("%C27*$t%C27%C12[%O$1%C12] %C14(%O$2@$3%C14) %O: $4%O")},

{"WhoIs Real Host", pevt_whoisrealhost_help, 4, 
N_("%C27*$t%C27%C12[%O$1%C12] %Oreal user@host %C11$2%O, real IP %C11$3%O")},

{"WhoIs Server Line", pevt_whois_server_help, 2, 
N_("%C27*$t%C27%C12[%O$1%C12]%O $2")},

{"WhoIs Special", pevt_whoisid_help, 3, 
N_("%C22*%O$t%C28[%O$1%C28]%O $2")},

{"You Join", pevt_ukick_help, 4, 
N_("%C27*$t%C27Now talking on %C11$2%O")},

{"You Kicked", pevt_ukick_help, 4, 
N_("%C27*$t%C27You have been kicked from $2 by $3 ($4%O)")},

{"You Part", pevt_part_help, 3, 
N_("%C27*$t%C27You have left channel $3")},

{"You Part with Reason", pevt_partreason_help, 4, 
N_("%C27*$t%C27You have left channel $3: $4")},

{"Your Action", pevt_chanaction_help, 131, 
"%C18*$t$1%O $2"},

{"Your Invitation", pevt_uinvite_help, 3, 
N_("%C27*$t%C27You've invited %C11$1%C to %C11$2%C (%C11$3%C)")},

{"Your Message", pevt_chanmsg_help, 4, 
N_("%C6%H<%H$1%H>%H%O$t$2%O")},

{"Your Nick Changing", pevt_uchangenick_help, 2, 
N_("%C27*$t%C27You are now known as $2")},

{"Netsplit", pevt_netsplit1_help, 3, 
N_("%C23*%O$t%C23Netsplit: $1 <-> $2, quits: $3")},
};
