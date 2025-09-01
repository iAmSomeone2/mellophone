use nom::{
    IResult,
    number::complete::{be_u32, le_u32},
};

pub enum CompressedTextureFormat {
    DXT1RGB,
    DXT1RGBA,
    DXT3RGBA,
    DXT5RGBA,
}

pub struct GPUTexture {
    pub width: u32,
    pub height: u32,
    pub img_buffer: Vec<u8>,
    pub mip_maps: Vec<GPUTexture>,
}

fn parse_le_u32_flags<F>(input: &[u8]) -> IResult<&[u8], F>
where
    F: bitflags::Flags<Bits = u32> + Default,
{
    let (rem, value) = le_u32(input)?;
    let flags = F::from_bits(value).unwrap_or_default();
    Ok((rem, flags))
}

#[derive(PartialEq, Debug, Clone, Copy, Eq)]
pub enum LoadTextureError {
    InvalidFormat,
    InvalidSize {
        expected: usize,
        actual: usize,
    },
}

impl std::fmt::Display for LoadTextureError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match self {
            Self::InvalidFormat => write!(f, "Invalid format"),
            Self::InvalidSize { expected, actual } => {
                write!(f, "Invalid size {actual} bytes, expected {expected} bytes")
            }
        }
    }
}

impl std::error::Error for LoadTextureError {}

trait Loadable {
    /// Returns `true` if the provided data meets the minimum requirements to describe the expected file type.
    ///
    /// **Note**: Passing this check does not guarantee that the data will successfully parse.
    fn is_file_type(input: &[u8]) -> bool;

    /// Loads the provided data into a GPUTexture.
    fn load(input: &[u8]) -> Result<Self, LoadTextureError>;
}

/// DirectDraw Surface file handling
mod dds {
    use super::*;
    use bitflags::bitflags;
    use nom::error::make_error;

    /// First 4 bytes of a DDS file
    const MAGIC_NUM: u32 = u32::from_be_bytes(*b"DDS ");
    // const MAGIC_NUM: u32 = 0x44445320;

    bitflags! {
        #[derive(Default, Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
        struct HeaderFlags: u32 {
            const Caps = 0x1;
            const Height = 0x2;
            const Width = 0x4;
            const Pitch = 0x8;
            const PixelFormat = 0x1000;
            const MipMapCount = 0x20000;
            const LinearSize = 0x80000;
            const Depth = 0x800000;

            const Texture = Self::Caps.bits() | Self::Height.bits() | Self::Width.bits() | Self::PixelFormat.bits();
            const MipMap = Self::MipMapCount.bits();
            const Volume = Self::Depth.bits();
        }

        /// Flags indicating what type of data is in the surface
        #[derive(Default, Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
        struct PixelFmtFlags: u32 {
            /// Texture contains alpha data; dwRGBAlphaBitMask contains valid data.
            const AlphaPixels = 0x1;
            /// Used in some older DDS files for alpha channel only uncompressed data (dwRGBBitCount contains the alpha channel bit-count; dwABitMask contains valid data)
            const Alpha = 0x2;
            /// Texture contains compressed RGB data; dwFourCC contains valid data.
            const FourCC = 0x4;
            /// Texture contains uncompressed RGB data; dwRGBBitCount and the RGB masks (dwRBitMask, dwGBitMask, dwBBitMask) contain valid data.
            const RGB = 0x40;
            /// Used in some older DDS files for YUV uncompressed data (dwRGBBitCount contains the YUV bit count; dwRBitMask contains the Y mask, dwGBitMask contains the U mask, dwBBitMask contains the V mask)
            const YUV = 0x200;
            /// Used in some older DDS files for single channel color uncompressed data (dwRGBBitCount contains the luminance channel bit-count; dwRBitMask contains the channel mask). Can be combined with DDPF_ALPHAPIXELS for a two-channel DDS file.
            const Luminance = 0x20000;
        }

        #[derive(Default, Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
        struct Capabilities: u32 {
            const Complex = 0x8;
            const MipMap = 0x400000;
            const Texture = 0x1000;

            const CubeMap = 0x200;
            const CubeMapPositiveX = 0x400;
            const CubeMapNegativeX = 0x800;
            const CubeMapPositiveY = 0x1000;
            const CubeMapNegativeY = 0x2000;
            const CubeMapPositiveZ = 0x4000;
            const CubeMapNegativeZ = 0x8000;

            const Volume = 0x200000;
        }
    }

    impl HeaderFlags {
        fn parse(input: &[u8]) -> IResult<&[u8], Self> {
            parse_le_u32_flags(input)
        }
    }

    impl PixelFmtFlags {
        fn parse(input: &[u8]) -> IResult<&[u8], Self> {
            parse_le_u32_flags(input)
        }
    }

    impl Capabilities {
        fn parse(input: &[u8]) -> IResult<&[u8], Self> {
            parse_le_u32_flags(input)
        }
    }

    /// Four-character long code indicating the type of texture surface contained in the file
    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    enum FourCharCode {
        DXT1,
        DXT2,
        DXT3,
        DXT4,
        DXT5,
        DX10,
    }

    impl FourCharCode {
        const fn into_u32(self) -> u32 {
            match self {
                FourCharCode::DXT1 => u32::from_be_bytes(*b"DXT1"),
                FourCharCode::DXT2 => u32::from_be_bytes(*b"DXT2"),
                FourCharCode::DXT3 => u32::from_be_bytes(*b"DXT3"),
                FourCharCode::DXT4 => u32::from_be_bytes(*b"DXT4"),
                FourCharCode::DXT5 => u32::from_be_bytes(*b"DXT5"),
                FourCharCode::DX10 => u32::from_be_bytes(*b"DX10"),
            }
        }
    }

    impl From<FourCharCode> for u32 {
        fn from(value: FourCharCode) -> Self {
            value.into_u32()
        }
    }

    impl TryFrom<u32> for FourCharCode {
        type Error = ();

        fn try_from(value: u32) -> Result<Self, Self::Error> {
            if value == FourCharCode::DXT1.into_u32() {
                Ok(FourCharCode::DXT1)
            } else if value == FourCharCode::DXT2.into_u32() {
                Ok(FourCharCode::DXT2)
            } else if value == FourCharCode::DXT3.into_u32() {
                Ok(FourCharCode::DXT3)
            } else if value == FourCharCode::DXT4.into_u32() {
                Ok(FourCharCode::DXT4)
            } else if value == FourCharCode::DXT5.into_u32() {
                Ok(FourCharCode::DXT5)
            } else if value == FourCharCode::DX10.into_u32() {
                Ok(FourCharCode::DX10)
            } else {
                Err(())
            }
        }
    }

    impl FourCharCode {
        fn parse(input: &[u8]) -> IResult<&[u8], FourCharCode> {
            let conv_err = nom::Err::Error(make_error(input, nom::error::ErrorKind::IsNot));

            let (rem, value) = be_u32(input)?;
            let char_code: FourCharCode = value.try_into().or(Err(conv_err))?;

            Ok((rem, char_code))
        }
    }

    /// DirectDraw Surface pixel format
    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    struct PixelFormat {
        struct_size: u32,
        flags: PixelFmtFlags,
        four_char_code: FourCharCode,
        rgb_bit_count: u32,
        red_mask: u32,
        green_mask: u32,
        blue_mask: u32,
        alpha_mask: u32,
    }

    impl PixelFormat {
        const STRUCT_SIZE: usize = 32;

        fn parse(input: &[u8]) -> IResult<&[u8], PixelFormat> {
            let (rem, struct_size) = le_u32(input)?;
            let (rem, flags) = PixelFmtFlags::parse(rem)?;
            let (rem, four_cc) = FourCharCode::parse(rem)?;
            let (rem, rgb_bit_count) = le_u32(rem)?;
            let (rem, red_mask) = le_u32(rem)?;
            let (rem, green_mask) = le_u32(rem)?;
            let (rem, blue_mask) = le_u32(rem)?;
            let (rem, alpha_mask) = le_u32(rem)?;

            Ok((
                rem,
                PixelFormat {
                    struct_size,
                    flags,
                    four_char_code: four_cc,
                    rgb_bit_count,
                    red_mask,
                    green_mask,
                    blue_mask,
                    alpha_mask,
                },
            ))
        }
    }

    /// Sizing scheme dependent on whether the texture is compressed or not
    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    enum TextureSizing {
        /// The pitch or number of bytes per scan line in an uncompressed texture
        Pitch(u32),
        /// The total number of bytes in the top level texture for a compressed texture.
        Linear(u32),
    }

    impl TextureSizing {
        fn parse(input: &[u8], header_flags: HeaderFlags) -> IResult<&[u8], TextureSizing> {
            let (rem, value) = le_u32(input)?;
            let sizing = if header_flags.contains(HeaderFlags::Pitch) {
                TextureSizing::Pitch(value)
            } else if header_flags.contains(HeaderFlags::LinearSize) {
                TextureSizing::Linear(value)
            } else {
                return Err(nom::Err::Error(make_error(
                    input,
                    nom::error::ErrorKind::Fail,
                )));
            };

            Ok((rem, sizing))
        }
    }

    /// DDS file header
    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    struct DDSHeader {
        /// Size of the file's header structure
        struct_size: u32,
        /// Flags indicating which header members contain valid data
        flags: HeaderFlags,
        /// Texture height (in pixels)
        height: u32,
        /// Texture width (in pixels)
        width: u32,
        /// Sizing scheme dependent on whether the texture is compressed or not
        texture_sizing: TextureSizing,
        /// Depth of a volume texture (in pixels)
        depth: Option<u32>,
        /// Number of mip maps levels
        mip_map_count: Option<u32>,
        pixel_format: PixelFormat,
        capabilities: Capabilities,
    }

    impl DDSHeader {
        const STRUCT_SIZE: usize = 124;

        fn parse<'p>(input: &'p [u8]) -> IResult<&'p [u8], Self> {
            let parse_optional = |input: &'p [u8],
                                  check_flag: HeaderFlags,
                                  header_flags: HeaderFlags|
             -> IResult<&'p [u8], Option<u32>> {
                // Parse u32 value regardless of flag since the headers are always the same size
                let (rem, value) = le_u32(input)?;
                let value = if header_flags.contains(check_flag) {
                    Some(value)
                } else {
                    None
                };

                Ok((rem, value))
            };

            let (rem, struct_size) = le_u32(input)?;
            let (rem, flags) = HeaderFlags::parse(rem)?;
            let (rem, height) = le_u32(rem)?;
            let (rem, width) = le_u32(rem)?;
            let (rem, texture_sizing) = TextureSizing::parse(rem, flags)?;
            let (rem, depth) = parse_optional(rem, HeaderFlags::Depth, flags)?;
            let (rem, mip_map_count) = parse_optional(rem, HeaderFlags::MipMapCount, flags)?;

            // Skip past the 44-byte reserved section
            let rem = &rem[44..];

            let (rem, pixel_format) = PixelFormat::parse(rem)?;

            // Parse the first two dwCaps sections and skip the last two
            let parse_caps_section =
                |input: &'p [u8], mut caps_acc: Capabilities| -> IResult<&'p [u8], Capabilities> {
                    let (rem, caps) = Capabilities::parse(input)?;
                    caps_acc |= caps;
                    Ok((rem, caps_acc))
                };

            let (rem, caps) = parse_caps_section(rem, Capabilities::default())?;
            let (rem, caps) = parse_caps_section(rem, caps)?;

            // Skip the last two sections (8 bytes) plus the final reserved DWORD (4 bytes) -- total 12 bytes
            let rem = &rem[12..];

            let header = Self {
                struct_size,
                flags,
                height,
                width,
                texture_sizing,
                depth,
                mip_map_count,
                pixel_format,
                capabilities: caps,
            };

            Ok((rem, header))
        }
    }

    /// DirectDraw Surface (DDS) file containing uncompressed or compressed DirectX textures.
    struct DirectDrawSurface {
        header: DDSHeader,
        /// Vector of bytes containing the main surface data
        main_surface: Vec<u8>,
        /// Vector of byte vectors containing data for any additional surfaces (mip maps, cube maps, or volumes)
        extra_surfaces: Vec<Vec<u8>>,
    }

    impl DirectDrawSurface {
        fn load(input: &[u8]) -> Result<Self, LoadTextureError> {
            let (rem, )
        }
    }

    #[cfg(test)]
    mod test {
        use super::*;
        use std::path::PathBuf;

        const ASSETS_DIR: &str = concat!(env!("CARGO_MANIFEST_DIR"), "/../", "assets");

        #[test]
        fn four_char_code_from_u32() {
            let test_cases = [
                (0x44585431u32, Ok(FourCharCode::DXT1)),
                (0x44585432, Ok(FourCharCode::DXT2)),
                (0x44585433, Ok(FourCharCode::DXT3)),
                (0x44585434, Ok(FourCharCode::DXT4)),
                (0x44585435, Ok(FourCharCode::DXT5)),
                (0x44583130, Ok(FourCharCode::DX10)),
                (0xAABBCCDD, Err(())),
            ];

            for (input, expected) in test_cases {
                assert_eq!(FourCharCode::try_from(input), expected);
            }
        }

        #[test]
        fn parse_dds_header() {
            let test_data = {
                let test_path = PathBuf::from(ASSETS_DIR)
                    .join("missing_album_cover.dds")
                    .canonicalize()
                    .unwrap();
                std::fs::read(&test_path).unwrap()
            };

            let parse_header_result = DDSHeader::parse(&test_data[4..]);
            assert!(parse_header_result.is_ok());
            let (_rem, header) = parse_header_result.unwrap();
            println!("{:?}", header);
        }
    }
}
