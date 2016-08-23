package vec

type Vector struct {
	X, Y, Z float64
}

func (v Vector) Dot(ov Vector) float64 {
	return v.X * ov.X + v.Y * ov.Y + v.Z * ov.Z
}

func (v Vector) Mult(f float64) Vector {
	var rv Vector
	rv.X = v.X * f
	rv.Y = v.Y * f
	rv.Z = v.Z * f
	return rv
}

func (v Vector) Add(ov Vector) Vector {
	var rv Vector
	rv.X = v.X + ov.X
	rv.Y = v.Y + ov.Y
	rv.Z = v.Z + ov.Z
	return rv
}
