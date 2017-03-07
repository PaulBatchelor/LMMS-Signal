#!/usr/bin/perl

use strict;
use 5.010;

my $name;
if($#ARGV < 1) {
    print ("Usage: sporth2xpf name file.sp\n");
    exit 1;
} 

$name = $ARGV[0];

open FILE, "<$ARGV[1]";

my $sporth;
while(<FILE>) {
    chomp;
    s/\"/&quot;/g;
    s/\'/&quot;/g;
    $sporth.= "$_&#xa;";
}
$sporth .= "dup";

close(FILE);

open(XPF, ">$name.xpf");

select(XPF);
print <<END
<?xml version="1.0"?>
<!DOCTYPE lmms-project>
<lmms-project version="1.0" creator="LMMS" creatorversion="1.1.90.524" type="instrumenttracksettings">
  <head/>
  <instrumenttracksettings muted="0" type="0" name="$name" solo="0">
    <instrumenttrack pan="0" fxch="0" usemasterpitch="1" pitchrange="1" pitch="0" basenote="57" vol="100">
      <instrument name="signal">
        <signal/>
      </instrument>
      <eldata fres="0.5" ftype="0" fcut="14000" fwet="0">
        <elvol lspd_denominator="4" sustain="0.5" pdel="0" userwavefile="" dec="0.5" lamt="0" latt="0" rel="0.1" amt="0" x100="0" att="0" lpdel="0" hold="0.5" lspd_syncmode="0" lshp="0" lspd="0.1" ctlenvamt="0" lspd_numerator="4"/>
        <elcut lspd_denominator="4" sustain="0.5" pdel="0" userwavefile="" dec="0.5" lamt="0" latt="0" rel="0.1" amt="0" x100="0" att="0" lpdel="0" hold="0.5" lspd_syncmode="0" lshp="0" lspd="0.1" ctlenvamt="0" lspd_numerator="4"/>
        <elres lspd_denominator="4" sustain="0.5" pdel="0" userwavefile="" dec="0.5" lamt="0" latt="0" rel="0.1" amt="0" x100="0" att="0" lpdel="0" hold="0.5" lspd_syncmode="0" lshp="0" lspd="0.1" ctlenvamt="0" lspd_numerator="4"/>
      </eldata>
      <chordcreator chord="0" chordrange="1" chord-enabled="0"/>
      <arpeggiator arptime="100" arprange="1" arpskip="0" arptime_denominator="4" arptime_syncmode="0" arpmode="0" arpcycle="0" arp-enabled="0" arp="0" arptime_numerator="4" arpdir="0" arpmiss="0" arpgate="100"/>
      <midiport inputcontroller="0" fixedoutputvelocity="-1" inputchannel="0" outputcontroller="0" writable="0" outputchannel="1" fixedinputvelocity="-1" fixedoutputnote="-1" outputprogram="1" basevelocity="63" readable="0"/>
      <fxchain numofeffects="1" enabled="1">
        <effect autoquit_syncmode="0" autoquit_numerator="4" autoquit_denominator="4" autoquit="1" gate="0" name="sporthedit" wet="-1" on="1">
          <SporthControls P0="0" P1="0" P2="0" P3="0" sporth="$sporth" compile="0"/>
          <key/>
        </effect>
      </fxchain>
    </instrumenttrack>
  </instrumenttracksettings>
</lmms-project>
END
;

close(XPF);

select STDOUT;

print("Wrote file to $name.xpf\n");
