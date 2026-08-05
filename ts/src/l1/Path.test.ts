// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Path } from './Path';

import { Converter } from './Converter';

type Point = { x: number; y: number } | null;
type Rect  = { x: number; y: number; width: number; height: number } | null;

class ParsePoint implements Converter<string, Point> {
    resolve(s: string): Point {
        const parts = s.split(',').map(Number);
        return parts.length === 2 && parts.every(Number.isFinite)
            ? { x: parts[0], y: parts[1] }
            : null;
    }
}

class ExpandToRect implements Converter<Point, Rect> {
    constructor(private readonly size: number) {}

    resolve(p: Point): Rect {
        return p && { x: p.x, y: p.y, width: this.size, height: this.size };
    }
}

describe('Path', () => {
    it('returns null by default when empty', () => {
        const path: Path<string, Rect> = new Path();
        assert.equal(path.fetch('10,20', {}), null);
    });

    it('walks a multi-step path from string to Rect', () => {
        const step1 = new ParsePoint();      // string -> Point
        const step2 = new ExpandToRect(100); // Point -> Rect

        // Factory vararg variant to make a Path
        const path = Path.create<string, Rect>(step1, step2);
        assert.deepEqual(path.fetch('10,20', {}), { x: 10, y: 20, width: 100, height: 100 });
        assert.equal(path.fetch('bad', {}), null);
    });

    it('fetch() and resolve() return the same result', () => {
        const path: Path<string, Rect> = new Path();
        path.append(new ParsePoint());
        path.append(new ExpandToRect(50));
        assert.deepEqual(path.fetch('5,10', {}), path.resolve('5,10', {}));
    });

    it('is assignable to Converter', () => {
        const path: Converter<string, Rect> = new Path();
        path.append(new ParsePoint());
        path.append(new ExpandToRect(10));
        assert.deepEqual(path.resolve('1,2', {}), { x: 1, y: 2, width: 10, height: 10 });
    });

    it('passes params through to each step', () => {
        type SizeParams = { size: number };

        class ExpandToRectParam implements Converter<Point, Rect, SizeParams> {
            resolve(p: Point, params: SizeParams): Rect {
                return p && { x: p.x, y: p.y, width: params.size, height: params.size };
            }
        }

        const path: Path<string, Rect, SizeParams> = new Path();
        path.append(new ParsePoint());
        path.append(new ExpandToRectParam());
        assert.deepEqual(path.fetch('3,4', { size: 20 }), { x: 3, y: 4, width: 20, height: 20 });
        assert.equal(path.fetch('bad', { size: 20 }), null);
    });

});
